/**
 * @file odyssey_model.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-11
 */

#include "odyssey_model.h"

#include <cstddef>

#include "odyssey_window.h"

namespace odyssey {

OdysseyModel::OdysseyModel(OdysseyWindow* window, QVulkanDeviceFunctions* deviceFuncs, const std::vector<Vertex>& vertices) : m_window(window), m_deviceFuncs(deviceFuncs) {
    createVertexBuffer(vertices);
}

OdysseyModel::~OdysseyModel() {
    m_deviceFuncs->vkDestroyBuffer(m_window->device(), vertexBuffer, nullptr);
    m_deviceFuncs->vkFreeMemory(m_window->device(), vertexBufferMemory, nullptr);
}

void OdysseyModel::bind(VkCommandBuffer& command_buffer) const {
    VkBuffer buffers[] = {vertexBuffer};
    m_deviceFuncs->vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, {0});
}

void OdysseyModel::draw(VkCommandBuffer& command_buffer) const {
    m_deviceFuncs->vkCmdDraw(command_buffer, vertexCount, 1, 0, 0);
}

void OdysseyModel::createVertexBuffer(const std::vector<Vertex>& vertices) {
    vertexCount = static_cast<uint32_t>(vertices.size());
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

    m_deviceFuncs->vkCreateBuffer(m_window->device(), )

    // engine_->CreateBuffer(
    //     buffer_size,
    //     vk::BufferUsageFlagBits::eVertexBuffer,
    //     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
    //     vertex_buffer_,
    //     vertex_buffer_memory_);
    // auto* data = engine_->Device().mapMemory(vertex_buffer_memory_, 0, buffer_size);
    // memcpy(data, vertices.data(), buffer_size);
    // engine_->Device().unmapMemory(vertex_buffer_memory_);
}

std::vector<VkVertexInputBindingDescription> OdysseyModel::Vertex::getBindingDescriptions() {
    return {};
    // std::vector<vk::VertexInputBindingDescription> binding_descriptions(1);
    // binding_descriptions.at(0)
    //     .setBinding(0)
    //     .setStride(sizeof(Vertex))
    //     .setInputRate(vk::VertexInputRate::eVertex);
    // return binding_descriptions;
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