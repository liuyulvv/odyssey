/**
 * @file odyssey_render.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_render.h"

#include <QVulkanDeviceFunctions>
#include <array>

#include "odyssey_window.h"

namespace odyssey {

OdysseyRender::OdysseyRender(OdysseyWindow* window) : m_window(window) {
}

void OdysseyRender::initResources() {
    m_deviceFuncs = m_window->vulkanInstance()->deviceFunctions(m_window->device());
}

void OdysseyRender::initSwapChainResources() {
}

void OdysseyRender::releaseSwapChainResources() {
}

void OdysseyRender::releaseResources() {
}

void OdysseyRender::startNextFrame() {
    VkClearColorValue clearColor = {{0.17F, 0.17F, 0.17F, 1.0F}};
    VkClearDepthStencilValue clearDepthStencil = {1.0F, 0};
    VkClearValue clearValues[2];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDepthStencil;

    VkRenderPassBeginInfo renderPassInfo;
    memset(&renderPassInfo, 0, sizeof(renderPassInfo));
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_window->defaultRenderPass();
    renderPassInfo.framebuffer = m_window->currentFramebuffer();
    auto size = m_window->swapChainImageSize();
    renderPassInfo.renderArea.extent.width = size.width();
    renderPassInfo.renderArea.extent.height = size.height();
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;
    VkCommandBuffer commandBuffer = m_window->currentCommandBuffer();
    m_deviceFuncs->vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    m_deviceFuncs->vkCmdEndRenderPass(commandBuffer);
    m_window->frameReady();
    m_window->requestUpdate();
}

}  // namespace odyssey