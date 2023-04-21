/**
 * @file odyssey_model.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-11
 */

#include "odyssey_model.h"

#include <array>
#include <cstddef>
#include <cstring>

#include "odyssey_device.h"

namespace odyssey {

OdysseyModel::OdysseyModel(OdysseyDevice* device, const std::vector<Vertex>& vertices) : m_device(device) {
    createVertexBuffer(vertices);
}

OdysseyModel::~OdysseyModel() {
    m_device->device().waitIdle();
    m_device->device().destroyBuffer(m_vertexBuffer);
    m_device->device().freeMemory(m_vertexBufferMemory);
}

void OdysseyModel::bind(vk::CommandBuffer& commandBuffer) const {
    std::array<vk::Buffer, 1> buffers{m_vertexBuffer};
    commandBuffer.bindVertexBuffers(0, buffers, {0});
}

void OdysseyModel::draw(vk::CommandBuffer& commandBuffer) const {
    commandBuffer.draw(m_vertexCount, 1, 0, 0);
}

void OdysseyModel::createVertexBuffer(const std::vector<Vertex>& vertices) {
    m_vertexCount = static_cast<uint32_t>(vertices.size());
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
    m_device->createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        m_vertexBuffer,
        m_vertexBufferMemory);
    auto* data = m_device->device().mapMemory(m_vertexBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), bufferSize);
    m_device->device().unmapMemory(m_vertexBufferMemory);
}

std::vector<vk::VertexInputBindingDescription> OdysseyModel::Vertex::getBindingDescriptions() {
    std::vector<vk::VertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions.at(0)
        .setBinding(0)
        .setStride(sizeof(Vertex))
        .setInputRate(vk::VertexInputRate::eVertex);
    return bindingDescriptions;
}

std::vector<vk::VertexInputAttributeDescription> OdysseyModel::Vertex::getAttributeDescriptions() {
    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(2);
    attributeDescriptions.at(0)
        .setBinding(0)
        .setLocation(0)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setOffset(offsetof(Vertex, position));
    attributeDescriptions.at(1)
        .setBinding(0)
        .setLocation(1)
        .setFormat(vk::Format::eR32G32B32A32Sfloat)
        .setOffset(offsetof(Vertex, color));
    return attributeDescriptions;
}

}  // namespace odyssey