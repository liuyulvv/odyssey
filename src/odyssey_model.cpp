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

OdysseyModel::OdysseyModel(OdysseyDevice* device, const Builder& builder) : m_device(device) {
    createVertexBuffer(builder.vertices);
    createIndexBuffer(builder.indices);
}

OdysseyModel::~OdysseyModel() {
    m_device->device().waitIdle();
    m_device->device().destroyBuffer(m_vertexBuffer);
    m_device->device().destroyBuffer(m_indexBuffer);
    if (m_hasIndexBuffer) {
        m_device->device().freeMemory(m_vertexBufferMemory);
        m_device->device().freeMemory(m_indexBufferMemory);
    }
}

void OdysseyModel::bind(vk::CommandBuffer& commandBuffer) const {
    std::array<vk::Buffer, 1> buffers{m_vertexBuffer};
    commandBuffer.bindVertexBuffers(0, buffers, {0});
    if (m_hasIndexBuffer) {
        commandBuffer.bindIndexBuffer(m_indexBuffer, 0, vk::IndexType::eUint32);
    }
}

void OdysseyModel::draw(vk::CommandBuffer& commandBuffer) const {
    if (m_hasIndexBuffer) {
        commandBuffer.drawIndexed(m_indexCount, 1, 0, 0, 0);
    } else {
        commandBuffer.draw(m_vertexCount, 1, 0, 0);
    }
}

void OdysseyModel::createVertexBuffer(const std::vector<Vertex>& vertices) {
    m_vertexCount = static_cast<uint32_t>(vertices.size());
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
    vk::Buffer stagingBuffer{};
    vk::DeviceMemory stagingBufferMemory{};
    m_device->createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);
    auto* data = m_device->device().mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), bufferSize);
    m_device->device().unmapMemory(stagingBufferMemory);
    m_device->createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        m_vertexBuffer,
        m_vertexBufferMemory);
    m_device->copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);
    m_device->device().destroyBuffer(stagingBuffer);
    m_device->device().freeMemory(stagingBufferMemory);
}

void OdysseyModel::createIndexBuffer(const std::vector<uint32_t>& indices) {
    m_indexCount = static_cast<uint32_t>(indices.size());
    m_hasIndexBuffer = !indices.empty();
    if (!m_hasIndexBuffer)
        return;
    vk::DeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;
    vk::Buffer stagingBuffer{};
    vk::DeviceMemory stagingBufferMemory{};
    m_device->createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);
    auto* data = m_device->device().mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, indices.data(), bufferSize);
    m_device->device().unmapMemory(stagingBufferMemory);
    m_device->createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        m_indexBuffer,
        m_indexBufferMemory);
    m_device->copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);
    m_device->device().destroyBuffer(stagingBuffer);
    m_device->device().freeMemory(stagingBufferMemory);
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
        .setFormat(vk::Format::eR32G32B32Sfloat)
        .setOffset(offsetof(Vertex, position));
    attributeDescriptions.at(1)
        .setBinding(0)
        .setLocation(1)
        .setFormat(vk::Format::eR32G32B32Sfloat)
        .setOffset(offsetof(Vertex, color));
    return attributeDescriptions;
}

}  // namespace odyssey