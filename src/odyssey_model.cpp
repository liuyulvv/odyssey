/**
 * @file odyssey_model.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-11
 */

#include "odyssey_model.h"

#include <cstddef>

#include "odyssey_window.h"

namespace odyssey {

OdysseyModel::OdysseyModel(OdysseyWindow* window, OdysseyWindowRender* render, QVulkanDeviceFunctions* deviceFuncs, const std::vector<Vertex>& vertices) : m_window(window), m_render(render), m_deviceFuncs(deviceFuncs) {
    createVertexBuffer(vertices);
}

OdysseyModel::~OdysseyModel() {
    m_deviceFuncs->vkDestroyBuffer(m_window->device(), m_buffer, nullptr);
    m_deviceFuncs->vkFreeMemory(m_window->device(), m_memory, nullptr);
}

void OdysseyModel::bind(VkCommandBuffer& command_buffer) const {
    VkBuffer buffers[] = {m_buffer};
    m_deviceFuncs->vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, {0});
}

void OdysseyModel::draw(VkCommandBuffer& command_buffer) const {
    m_deviceFuncs->vkCmdDraw(command_buffer, m_vertexCount, 1, 0, 0);
}

void OdysseyModel::createVertexBuffer(const std::vector<Vertex>& vertices) {
    m_vertexCount = static_cast<uint32_t>(vertices.size());
    VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;

    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory{};
    m_render->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    void* data;
    m_deviceFuncs->vkMapMemory(m_window->device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    m_deviceFuncs->vkUnmapMemory(m_window->device(), stagingBufferMemory);
    m_render->createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &m_buffer, &m_memory);
    m_render->copyBuffer(stagingBuffer, m_buffer, bufferSize);
    m_deviceFuncs->vkDestroyBuffer(m_window->device(), stagingBuffer, nullptr);
    m_deviceFuncs->vkFreeMemory(m_window->device(), stagingBufferMemory, nullptr);
}

std::vector<VkVertexInputBindingDescription> OdysseyModel::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions.at(0).binding = 0;
    bindingDescriptions.at(0).stride = sizeof(Vertex);
    bindingDescriptions.at(0).inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> OdysseyModel::Vertex::getAttributeDescriptions() {
    return {};
    // std::vector<vk::VertexInputAttributeDescription> attribute_descriptions(2);
    // attribute_descriptions.at(0)
    //     .setBinding(0)
    //     .setLocation(0)
    //     .setFormat(vk::Format::eR32G32Sfloat)
    //     .setOffset(offsetof(Vertex, position_));
    // attribute_descriptions.at(1)
    //     .setBinding(0)
    //     .setLocation(1)
    //     .setFormat(vk::Format::eR32G32B32A32Sfloat)
    //     .setOffset(offsetof(Vertex, color_));
    // return attribute_descriptions;
}

}  // namespace odyssey