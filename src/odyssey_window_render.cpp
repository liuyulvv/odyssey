/**
 * @file odyssey_window_render.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_window_render.h"

#include <QtConcurrentRun>

#include "odyssey_pipeline.h"
#include "odyssey_window.h"

namespace odyssey {

OdysseyWindowRender::OdysseyWindowRender(OdysseyWindow* window) : m_window(window) {
}

void OdysseyWindowRender::initResources() {
    m_deviceFuncs = m_window->vulkanInstance()->deviceFunctions(m_window->device());
    // createPipelineLayout();
    // createPipeline("shaders/shader.vert.spv", "shaders/shader.frag.spv", VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
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

void OdysseyWindowRender::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* memory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    m_deviceFuncs->vkCreateBuffer(m_window->device(), &bufferInfo, nullptr, buffer);
    VkMemoryRequirements memoryRequireMents{};
    m_deviceFuncs->vkGetBufferMemoryRequirements(m_window->device(), *buffer, &memoryRequireMents);
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequireMents.size;
    allocateInfo.memoryTypeIndex = findMemoryType(memoryRequireMents.memoryTypeBits, properties);
    m_deviceFuncs->vkAllocateMemory(m_window->device(), &allocateInfo, nullptr, memory);
    m_deviceFuncs->vkBindBufferMemory(m_window->device(), *buffer, *memory, 0);
}

void OdysseyWindowRender::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {
    auto commandBuffer = beginSingleTimeCommands();
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    m_deviceFuncs->vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);
    endSingleTimeCommands(commandBuffer);
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

void OdysseyWindowRender::createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, VkPrimitiveTopology primitiveTopology) {
    auto pipelineConfig = OdysseyPipeline::defaultPipelineConfigInfo(primitiveTopology);
    pipelineConfig.m_renderPass = m_window->defaultRenderPass();
    pipelineConfig.m_pipelineLayout = m_pipelineLayout;
    m_pipelines.push_back(new OdysseyPipeline(m_window, m_deviceFuncs, vertShaderPath, fragShaderPath, pipelineConfig));
}

uint32_t OdysseyWindowRender::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(m_window->physicalDevice(), &memoryProperties);
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type.");
}

VkCommandBuffer OdysseyWindowRender::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = m_window->graphicsCommandPool();
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer{};
    m_deviceFuncs->vkAllocateCommandBuffers(m_window->device(), &allocateInfo, &commandBuffer);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    m_deviceFuncs->vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void OdysseyWindowRender::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    m_deviceFuncs->vkEndCommandBuffer(commandBuffer);
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    m_deviceFuncs->vkQueueSubmit(m_window->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    m_deviceFuncs->vkQueueWaitIdle(m_window->graphicsQueue());
    m_deviceFuncs->vkFreeCommandBuffers(m_window->device(), m_window->graphicsCommandPool(), 1, &commandBuffer);
}

}  // namespace odyssey