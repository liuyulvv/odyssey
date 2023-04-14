/**
 * @file odyssey_window_render.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_window_render.h"

#include "odyssey_pipeline.h"
#include "odyssey_window.h"

namespace odyssey {

OdysseyWindowRender::OdysseyWindowRender(OdysseyWindow* window) : m_window(window) {
}

void OdysseyWindowRender::initResources() {
    m_deviceFuncs = m_window->vulkanInstance()->deviceFunctions(m_window->device());
    createPipelineLayout();
}

void OdysseyWindowRender::initSwapChainResources() {
}

void OdysseyWindowRender::releaseSwapChainResources() {
}

void OdysseyWindowRender::releaseResources() {
}

void OdysseyWindowRender::startNextFrame() {
    VkClearColorValue clearColor = {{0.17F, 0.17F, 0.17F, 1.0F}};
    VkClearDepthStencilValue clearDepthStencil = {1.0F, 0};
    VkClearValue clearValues[2]{};
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDepthStencil;

    VkRenderPassBeginInfo renderPassInfo{};
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

void OdysseyWindowRender::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineInfo.setLayoutCount = 0;
    pipelineInfo.pSetLayouts = nullptr;
    pipelineInfo.pushConstantRangeCount = 0;
    pipelineInfo.pPushConstantRanges = nullptr;
    m_deviceFuncs->vkCreatePipelineLayout(m_window->device(), &pipelineInfo, nullptr, &m_pipelineLayout);
}

OdysseyPipeline* OdysseyWindowRender::createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, VkPrimitiveTopology primitiveTopology) {
    auto pipelineConfig = OdysseyPipeline::defaultPipelineConfigInfo(primitiveTopology);
    pipelineConfig.renderPass = m_window->defaultRenderPass();
    pipelineConfig.pipelineLayout = m_pipelineLayout;
    return new OdysseyPipeline(m_window, m_deviceFuncs, vertShaderPath, fragShaderPath, pipelineConfig);
}

void OdysseyWindowRender::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
}

}  // namespace odyssey