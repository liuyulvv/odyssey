/**
 * @file odyssey_swap_chain.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-10
 */

#include "odyssey_swap_chain.h"

#include <array>

namespace odyssey {

OdysseySwapChain::OdysseySwapChain(std::shared_ptr<OdysseyEngine> engine, vk::Extent2D window_extent) : engine_(std::move(engine)), window_extent_(window_extent) {
    CreateSwapChain();
    CreateRenderPass();
    CreateDepthResources();
    CreateFrameBuffers();
    CreateSyncObjects();
}

OdysseySwapChain::~OdysseySwapChain() {
    for (auto image_view : swap_chain_image_views_) {
        engine_->Device().destroyImageView(image_view);
    }
    swap_chain_image_views_.clear();
    if (swap_chain_) {
        engine_->Device().destroySwapchainKHR(swap_chain_);
        swap_chain_ = nullptr;
    }
    for (size_t i = 0; i < depth_images_.size(); ++i) {
        engine_->Device().destroyImageView(depth_image_views_[i]);
        engine_->Device().destroyImage(depth_images_[i]);
        engine_->Device().freeMemory(depth_image_memories_[i]);
    }
    for (auto& framebuffer : swap_chain_frame_buffers_) {
        engine_->Device().destroyFramebuffer(framebuffer);
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT_; ++i) {
        engine_->Device().destroySemaphore(render_finished_semaphores_[i]);
        engine_->Device().destroySemaphore(image_available_semaphores_[i]);
        engine_->Device().destroyFence(in_flight_fences_[i]);
    }
    engine_->Device().destroyRenderPass(render_pass_);
}

const vk::Format& OdysseySwapChain::GetSwapChainImageFormat() const {
    return swap_chain_image_format_;
}

size_t OdysseySwapChain::GetImageCount() const {
    return swap_chain_images_.size();
}

vk::Extent2D OdysseySwapChain::GetSwapChainExtent() const {
    return swap_chain_extent_;
}

const vk::Framebuffer& OdysseySwapChain::GetFrameBuffer(size_t index) const {
    return swap_chain_frame_buffers_[index];
}

const vk::ImageView& OdysseySwapChain::GetImageView(size_t index) const {
    return swap_chain_image_views_[index];
}

const vk::RenderPass& OdysseySwapChain::GetRenderPass() const {
    return render_pass_;
}

uint32_t OdysseySwapChain::GetWidth() const {
    return swap_chain_extent_.width;
}

uint32_t OdysseySwapChain::GetHeight() const {
    return swap_chain_extent_.height;
}

float OdysseySwapChain::GetExtentAspectRatio() const {
    return static_cast<float>(swap_chain_extent_.width) / static_cast<float>(swap_chain_extent_.height);
}

uint32_t OdysseySwapChain::AcquireNextImage() {
    engine_->Device().waitForFences(in_flight_fences_[current_frame_], true, std::numeric_limits<uint64_t>::max());
    return engine_->Device().acquireNextImageKHR(swap_chain_, std::numeric_limits<uint64_t>::max(), image_available_semaphores_[current_frame_], nullptr).value;
}

void OdysseySwapChain::SubmitCommandBuffers(const vk::CommandBuffer& buffer, uint32_t& image_index) {
    if (images_in_flight_[image_index]) {
        engine_->Device().waitForFences(images_in_flight_[image_index], true, std::numeric_limits<uint64_t>::max());
    }
    images_in_flight_[image_index] = in_flight_fences_[current_frame_];

    vk::SubmitInfo submit_info;
    vk::PipelineStageFlags wait_dst_stage_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit_info
        .setWaitSemaphoreCount(1)
        .setWaitSemaphores(image_available_semaphores_[current_frame_])
        .setWaitDstStageMask(wait_dst_stage_mask)
        .setCommandBufferCount(1)
        .setCommandBuffers(buffer)
        .setSignalSemaphoreCount(1)
        .setSignalSemaphores(render_finished_semaphores_[current_frame_]);

    engine_->Device().resetFences(in_flight_fences_[current_frame_]);

    engine_->GetGraphicsQueue().submit(submit_info, in_flight_fences_[current_frame_]);

    vk::PresentInfoKHR present_info;
    present_info
        .setWaitSemaphoreCount(1)
        .setWaitSemaphores(render_finished_semaphores_[current_frame_])
        .setSwapchainCount(1)
        .setSwapchains(swap_chain_)
        .setImageIndices(image_index);

    engine_->GetPresentQueue().presentKHR(present_info);
    current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT_;
}

void OdysseySwapChain::CreateSwapChain() {
    auto swap_chain_support = engine_->GetSwapChainSupport();
    auto surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats_);
    swap_chain_image_format_ = surface_format.format;
    auto present_mode = ChooseSwapPresentMode(swap_chain_support.present_modes_);
    auto extent = ChooseSwapExtent(swap_chain_support.capabilities_);
    swap_chain_extent_ = extent;
    uint32_t image_count = swap_chain_support.capabilities_.minImageCount + 1;
    if (swap_chain_support.capabilities_.maxImageCount > 0 && image_count > swap_chain_support.capabilities_.maxImageCount) {
        image_count = swap_chain_support.capabilities_.maxImageCount;
    }
    vk::SwapchainCreateInfoKHR create_info{};
    create_info
        .setSurface(engine_->Surface())
        .setMinImageCount(image_count)
        .setImageFormat(surface_format.format)
        .setImageColorSpace(surface_format.colorSpace)
        .setImageExtent(extent)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setPreTransform(swap_chain_support.capabilities_.currentTransform)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(present_mode)
        .setClipped(true);
    auto indices = engine_->FindPhysicalQueueFamilies();
    if (indices.graphics_family_ != indices.present_family_) {
        std::array<uint32_t, 2> queue_family_indices{indices.graphics_family_, indices.present_family_};
        create_info
            .setImageSharingMode(vk::SharingMode::eConcurrent)
            .setQueueFamilyIndices(queue_family_indices);
    } else {
        create_info
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setQueueFamilyIndices(indices.graphics_family_);
    }
    swap_chain_ = engine_->Device().createSwapchainKHR(create_info);
    swap_chain_images_ = engine_->Device().getSwapchainImagesKHR(swap_chain_);
    swap_chain_image_views_.resize(swap_chain_images_.size());
    for (size_t i = 0; i < swap_chain_images_.size(); ++i) {
        swap_chain_image_views_[i] = engine_->CreateImageView(swap_chain_images_[i], swap_chain_image_format_, vk::ImageAspectFlagBits::eColor);
    }
}

void OdysseySwapChain::CreateRenderPass() {
    vk::AttachmentDescription depth_attachment{};
    depth_attachment
        .setFormat(FindDepthFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::AttachmentReference depth_attachment_reference;
    depth_attachment_reference
        .setAttachment(1)
        .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentDescription color_attachment;
    color_attachment
        .setFormat(GetSwapChainImageFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    vk::AttachmentReference color_attachment_reference;
    color_attachment_reference
        .setAttachment(0)
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachmentCount(1)
        .setColorAttachments(color_attachment_reference)
        .setPDepthStencilAttachment(&depth_attachment_reference);

    vk::SubpassDependency dependency;
    dependency
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstSubpass(0)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

    std::array<vk::AttachmentDescription, 2> attachments{color_attachment, depth_attachment};

    vk::RenderPassCreateInfo render_pass_info;
    render_pass_info
        .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
        .setAttachments(attachments)
        .setSubpassCount(1)
        .setSubpasses(subpass)
        .setDependencyCount(1)
        .setDependencies(dependency);
    render_pass_ = engine_->Device().createRenderPass(render_pass_info);
}

void OdysseySwapChain::CreateDepthResources() {
    auto depth_format = FindDepthFormat();
    auto swap_chain_extent = GetSwapChainExtent();
    depth_images_.resize(GetImageCount());
    depth_image_memories_.resize(GetImageCount());
    depth_image_views_.resize(GetImageCount());
    for (size_t i = 0; i < depth_images_.size(); ++i) {
        engine_->CreateImage(swap_chain_extent.width, swap_chain_extent.height, depth_format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, depth_images_[i], depth_image_memories_[i]);
        depth_image_views_[i] = engine_->CreateImageView(depth_images_[i], depth_format, vk::ImageAspectFlagBits::eDepth);
    }
}

void OdysseySwapChain::CreateFrameBuffers() {
    swap_chain_frame_buffers_.resize(GetImageCount());
    for (size_t i = 0; i < GetImageCount(); ++i) {
        std::array<vk::ImageView, 2> attachments{swap_chain_image_views_[i], depth_image_views_[i]};
        auto swap_chain_extent = GetSwapChainExtent();
        vk::FramebufferCreateInfo framebuffer_info{};
        framebuffer_info
            .setRenderPass(render_pass_)
            .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
            .setAttachments(attachments)
            .setWidth(swap_chain_extent.width)
            .setHeight(swap_chain_extent.height)
            .setLayers(1);

        swap_chain_frame_buffers_[i] = engine_->Device().createFramebuffer(framebuffer_info);
    }
}

void OdysseySwapChain::CreateSyncObjects() {
    image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT_);
    render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT_);
    in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT_);
    images_in_flight_.resize(GetImageCount());

    vk::SemaphoreCreateInfo semaphore_info{};
    vk::FenceCreateInfo fence_info{};
    fence_info.setFlags(vk::FenceCreateFlagBits::eSignaled);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT_; ++i) {
        image_available_semaphores_[i] = engine_->Device().createSemaphore(semaphore_info);
        render_finished_semaphores_[i] = engine_->Device().createSemaphore(semaphore_info);
        in_flight_fences_[i] = engine_->Device().createFence(fence_info);
    }
}

vk::SurfaceFormatKHR OdysseySwapChain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats) {
    for (const auto& available_format : available_formats) {
        if (available_format.format == vk::Format::eR8G8B8A8Srgb && available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return available_format;
        }
    }
    return available_formats[0];
}

vk::PresentModeKHR OdysseySwapChain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes) {
    for (const auto& available_present_mode : available_present_modes) {
        if (available_present_mode == vk::PresentModeKHR::eMailbox) {
            return available_present_mode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D OdysseySwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    vk::Extent2D actual_extent = window_extent_;
    actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));
    return actual_extent;
}

vk::Format OdysseySwapChain::FindDepthFormat() const {
    return engine_->FindSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

}  // namespace odyssey
