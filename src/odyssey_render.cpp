/**
 * @file odyssey_render.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-21
 */

#include "odyssey_render.h"

#include "odyssey_swap_chain.h"

namespace odyssey {

OdysseyRender::OdysseyRender(OdysseyWindow* window, OdysseyDevice* device) : m_window(window), m_device(device) {
    recreateSwapChain();
    createCommandBuffers();
}

OdysseyRender::~OdysseyRender() {
    m_device->device().waitIdle();
    m_swapChain.reset();
    freeCommandBuffers();
}

const vk::RenderPass& OdysseyRender::getSwapChainRenderPass() const {
    return m_swapChain->getRenderPass();
}

bool OdysseyRender::isFrameInProgress() const {
    return m_isFrameStarted;
}

vk::CommandBuffer OdysseyRender::getCurrentCommandBuffer() const {
    return m_commandBuffers[m_currentImageIndex];
}

vk::CommandBuffer OdysseyRender::beginFrame() {
    try {
        m_currentImageIndex = m_swapChain->acquireNextImage();
        m_isFrameStarted = true;
        auto commandBuffer = getCurrentCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo{};
        commandBuffer.begin(beginInfo);
        return commandBuffer;
    } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
        recreateSwapChain();
        return nullptr;
    }
}

void OdysseyRender::endFrame() {
    try {
        auto commandBuffer = getCurrentCommandBuffer();
        commandBuffer.end();
        m_swapChain->submitCommandBuffers(commandBuffer, m_currentImageIndex);
        m_isFrameStarted = false;
    } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
        recreateSwapChain();
    }
}

void OdysseyRender::beginSwapChainRenderPass(vk::CommandBuffer commandBuffer) {
    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo
        .setRenderPass(m_swapChain->getRenderPass())
        .setFramebuffer(m_swapChain->getFrameBuffer(m_currentImageIndex));
    renderPassInfo.renderArea
        .setOffset({0, 0})
        .setExtent(m_swapChain->getSwapChainExtent());
    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].setColor({0.17F, 0.17F, 0.17F, 1.0F});
    clearValues[1].setDepthStencil({1.0F, 0});
    renderPassInfo
        .setClearValueCount(static_cast<uint32_t>(clearValues.size()))
        .setClearValues(clearValues);
    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    vk::Viewport viewport{};
    viewport
        .setX(0.0F)
        .setY(0.0F)
        .setWidth(static_cast<float>(m_swapChain->getSwapChainExtent().width))
        .setHeight(static_cast<float>(m_swapChain->getSwapChainExtent().height))
        .setMinDepth(0.0F)
        .setMaxDepth(1.0F);
    vk::Rect2D scissor{{0, 0}, m_swapChain->getSwapChainExtent()};
    commandBuffer.setViewport(0, viewport);
    commandBuffer.setScissor(0, scissor);
}

void OdysseyRender::endSwapChainRenderPass(vk::CommandBuffer commandBuffer) {
    commandBuffer.endRenderPass();
}

void OdysseyRender::recreateSwapChain() {
    m_device->device().waitIdle();
    m_swapChain.reset(nullptr);
    m_swapChain = std::make_unique<OdysseySwapChain>(m_device, m_window->width(), m_window->height());
}

void OdysseyRender::createCommandBuffers() {
    m_commandBuffers.resize(m_swapChain->getImageCount());
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(m_device->getCommandPool())
        .setCommandBufferCount(static_cast<uint32_t>(m_commandBuffers.size()));
    m_commandBuffers = m_device->device().allocateCommandBuffers(allocInfo);
}

void OdysseyRender::freeCommandBuffers() {
    m_device->device().freeCommandBuffers(m_device->getCommandPool(), m_commandBuffers);
}

}  // namespace odyssey