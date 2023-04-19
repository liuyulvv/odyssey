/**
 * @file odyssey_swap_chain.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-10
 */

#include "odyssey_swap_chain.h"

#include <array>
#include <limits>

namespace odyssey {

OdysseySwapChain::OdysseySwapChain(OdysseyEngine* engine, int width, int height) : m_engine(engine) {
    m_windowExtent.setWidth(width);
    m_windowExtent.setHeight(height);
    createSwapChain();
    createRenderPass();
    createDepthResources();
    createFrameBuffers();
    createSyncObjects();
}

OdysseySwapChain::~OdysseySwapChain() {
    for (auto imageView : m_swapChainImageViews) {
        m_engine->device().destroyImageView(imageView);
    }
    m_swapChainImageViews.clear();
    if (m_swapChain) {
        m_engine->device().destroySwapchainKHR(m_swapChain);
        m_swapChain = nullptr;
    }
    for (size_t i = 0; i < m_depthImages.size(); ++i) {
        m_engine->device().destroyImageView(m_depthImageViews[i]);
        m_engine->device().destroyImage(m_depthImages[i]);
        m_engine->device().freeMemory(m_depthImageMemories[i]);
    }
    for (auto& framebuffer : m_swapChainFrameBuffers) {
        m_engine->device().destroyFramebuffer(framebuffer);
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        m_engine->device().destroySemaphore(m_renderFinishedSemaphores[i]);
        m_engine->device().destroySemaphore(m_imageAvailableSemaphores[i]);
        m_engine->device().destroyFence(m_inFlightFences[i]);
    }
    m_engine->device().destroyRenderPass(m_renderPass);
}

const vk::Format& OdysseySwapChain::getSwapChainImageFormat() const {
    return m_swapChainImageFormat;
}

size_t OdysseySwapChain::getImageCount() const {
    return m_swapChainImages.size();
}

vk::Extent2D OdysseySwapChain::getSwapChainExtent() const {
    return m_swapChainExtent;
}

const vk::Framebuffer& OdysseySwapChain::getFrameBuffer(size_t index) const {
    return m_swapChainFrameBuffers[index];
}

const vk::ImageView& OdysseySwapChain::getImageView(size_t index) const {
    return m_swapChainImageViews[index];
}

const vk::RenderPass& OdysseySwapChain::getRenderPass() const {
    return m_renderPass;
}

uint32_t OdysseySwapChain::getWidth() const {
    return m_swapChainExtent.width;
}

uint32_t OdysseySwapChain::getHeight() const {
    return m_swapChainExtent.height;
}

float OdysseySwapChain::getExtentAspectRatio() const {
    return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
}

uint32_t OdysseySwapChain::acquireNextImage() {
    m_engine->device().waitForFences(m_inFlightFences[m_currentFrame], true, (std::numeric_limits<uint64_t>::max)());
    return m_engine->device().acquireNextImageKHR(m_swapChain, (std::numeric_limits<uint64_t>::max)(), m_imageAvailableSemaphores[m_currentFrame], nullptr).value;
}

void OdysseySwapChain::submitCommandBuffers(const vk::CommandBuffer& buffer, uint32_t imageIndex) {
    if (m_imagesInFlight[imageIndex]) {
        m_engine->device().waitForFences(m_imagesInFlight[imageIndex], true, (std::numeric_limits<uint64_t>::max)());
    }
    m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];

    vk::SubmitInfo submitInfo;
    vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submitInfo
        .setWaitSemaphoreCount(1)
        .setWaitSemaphores(m_imageAvailableSemaphores[m_currentFrame])
        .setWaitDstStageMask(waitDstStageMask)
        .setCommandBufferCount(1)
        .setCommandBuffers(buffer)
        .setSignalSemaphoreCount(1)
        .setSignalSemaphores(m_renderFinishedSemaphores[m_currentFrame]);

    m_engine->device().resetFences(m_inFlightFences[m_currentFrame]);

    m_engine->getGraphicsQueue().submit(submitInfo, m_inFlightFences[m_currentFrame]);

    vk::PresentInfoKHR presentInfo;
    presentInfo
        .setWaitSemaphoreCount(1)
        .setWaitSemaphores(m_renderFinishedSemaphores[m_currentFrame])
        .setSwapchainCount(1)
        .setSwapchains(m_swapChain)
        .setImageIndices(imageIndex);

    m_engine->getPresentQueue().presentKHR(presentInfo);
    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void OdysseySwapChain::createSwapChain() {
    auto swapChainSupport = m_engine->getSwapChainSupport();
    auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    m_swapChainImageFormat = surfaceFormat.format;
    auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    auto extent = chooseSwapExtent(swapChainSupport.capabilities);
    m_swapChainExtent = extent;
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    vk::SwapchainCreateInfoKHR createinfo{};
    createinfo
        .setSurface(m_engine->surface())
        .setMinImageCount(imageCount)
        .setImageFormat(surfaceFormat.format)
        .setImageColorSpace(surfaceFormat.colorSpace)
        .setImageExtent(extent)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setPreTransform(swapChainSupport.capabilities.currentTransform)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(presentMode)
        .setClipped(true);
    auto indices = m_engine->findPhysicalQueueFamilies();
    if (indices.graphicsFamily != indices.presentFamily) {
        std::array<uint32_t, 2> queueFamilyIndices{indices.graphicsFamily, indices.presentFamily};
        createinfo
            .setImageSharingMode(vk::SharingMode::eConcurrent)
            .setQueueFamilyIndices(queueFamilyIndices);
    } else {
        createinfo
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setQueueFamilyIndices(indices.graphicsFamily);
    }
    m_swapChain = m_engine->device().createSwapchainKHR(createinfo);
    m_swapChainImages = m_engine->device().getSwapchainImagesKHR(m_swapChain);
    m_swapChainImageViews.resize(m_swapChainImages.size());
    for (size_t i = 0; i < m_swapChainImages.size(); ++i) {
        m_swapChainImageViews[i] = m_engine->createImageView(m_swapChainImages[i], m_swapChainImageFormat, vk::ImageAspectFlagBits::eColor);
    }
}

void OdysseySwapChain::createRenderPass() {
    vk::AttachmentDescription depthAttachment{};
    depthAttachment
        .setFormat(findDepthFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::AttachmentReference depthAttachmentReference;
    depthAttachmentReference
        .setAttachment(1)
        .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentDescription colorAttachment;
    colorAttachment
        .setFormat(getSwapChainImageFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    vk::AttachmentReference colorAttachmentReference;
    colorAttachmentReference
        .setAttachment(0)
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachmentCount(1)
        .setColorAttachments(colorAttachmentReference)
        .setPDepthStencilAttachment(&depthAttachmentReference);

    vk::SubpassDependency dependency;
    dependency
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstSubpass(0)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

    std::array<vk::AttachmentDescription, 2> attachments{colorAttachment, depthAttachment};

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo
        .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
        .setAttachments(attachments)
        .setSubpassCount(1)
        .setSubpasses(subpass)
        .setDependencyCount(1)
        .setDependencies(dependency);
    m_renderPass = m_engine->device().createRenderPass(renderPassInfo);
}

void OdysseySwapChain::createDepthResources() {
    auto depthFormat = findDepthFormat();
    auto swapChainExtent = getSwapChainExtent();
    m_depthImages.resize(getImageCount());
    m_depthImageMemories.resize(getImageCount());
    m_depthImageViews.resize(getImageCount());
    for (size_t i = 0; i < m_depthImages.size(); ++i) {
        m_engine->createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, m_depthImages[i], m_depthImageMemories[i]);
        m_depthImageViews[i] = m_engine->createImageView(m_depthImages[i], depthFormat, vk::ImageAspectFlagBits::eDepth);
    }
}

void OdysseySwapChain::createFrameBuffers() {
    m_swapChainFrameBuffers.resize(getImageCount());
    for (size_t i = 0; i < getImageCount(); ++i) {
        std::array<vk::ImageView, 2> attachments{m_swapChainImageViews[i], m_depthImageViews[i]};
        auto swapChainExtent = getSwapChainExtent();
        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo
            .setRenderPass(m_renderPass)
            .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
            .setAttachments(attachments)
            .setWidth(swapChainExtent.width)
            .setHeight(swapChainExtent.height)
            .setLayers(1);
        m_swapChainFrameBuffers[i] = m_engine->device().createFramebuffer(framebufferInfo);
    }
}

void OdysseySwapChain::createSyncObjects() {
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    m_imagesInFlight.resize(getImageCount());
    vk::SemaphoreCreateInfo semaphoreInfo{};
    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        m_imageAvailableSemaphores[i] = m_engine->device().createSemaphore(semaphoreInfo);
        m_renderFinishedSemaphores[i] = m_engine->device().createSemaphore(semaphoreInfo);
        m_inFlightFences[i] = m_engine->device().createFence(fenceInfo);
    }
}

vk::SurfaceFormatKHR OdysseySwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eR8G8B8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

vk::PresentModeKHR OdysseySwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D OdysseySwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
        return capabilities.currentExtent;
    }
    vk::Extent2D actualExtent = m_windowExtent;
    actualExtent.width = (std::max)(capabilities.minImageExtent.width, (std::min)(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = (std::max)(capabilities.minImageExtent.height, (std::min)(capabilities.maxImageExtent.height, actualExtent.height));
    return actualExtent;
}

vk::Format OdysseySwapChain::findDepthFormat() const {
    return m_engine->findSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

}  // namespace odyssey
