/**
 * @file odyssey_model.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-11
 */

#include "odyssey_model.h"

#include <array>
#include <cstddef>
#include <cstring>

namespace odyssey {

OdysseyModel::OdysseyModel(OdysseyEngine* engine, const std::vector<Vertex>& vertices) : engine_(engine) {
    CreateVertexBuffer(vertices);
}

OdysseyModel::~OdysseyModel() {
    engine_->Device().destroyBuffer(vertex_buffer_);
    engine_->Device().freeMemory(vertex_buffer_memory_);
}

void OdysseyModel::Bind(vk::CommandBuffer& command_buffer) const {
    std::array<vk::Buffer, 1> buffers{vertex_buffer_};
    command_buffer.bindVertexBuffers(0, buffers, {0});
}

void OdysseyModel::Draw(vk::CommandBuffer& command_buffer) const {
    command_buffer.draw(vertex_count_, 1, 0, 0);
}

void OdysseyModel::CreateVertexBuffer(const std::vector<Vertex>& vertices) {
    vertex_count_ = static_cast<uint32_t>(vertices.size());
    vk::DeviceSize buffer_size = sizeof(vertices[0]) * vertex_count_;
    engine_->CreateBuffer(
        buffer_size,
        vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        vertex_buffer_,
        vertex_buffer_memory_);
    auto* data = engine_->Device().mapMemory(vertex_buffer_memory_, 0, buffer_size);
    memcpy(data, vertices.data(), buffer_size);
    engine_->Device().unmapMemory(vertex_buffer_memory_);
}

std::vector<vk::VertexInputBindingDescription> OdysseyModel::Vertex::GetBindingDescriptions() {
    std::vector<vk::VertexInputBindingDescription> binding_descriptions(1);
    binding_descriptions.at(0)
        .setBinding(0)
        .setStride(sizeof(Vertex))
        .setInputRate(vk::VertexInputRate::eVertex);
    return binding_descriptions;
}

std::vector<vk::VertexInputAttributeDescription> OdysseyModel::Vertex::GetAttributeDescriptions() {
    std::vector<vk::VertexInputAttributeDescription> attribute_descriptions(2);
    attribute_descriptions.at(0)
        .setBinding(0)
        .setLocation(0)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setOffset(offsetof(Vertex, position_));
    attribute_descriptions.at(1)
        .setBinding(0)
        .setLocation(1)
        .setFormat(vk::Format::eR32G32B32A32Sfloat)
        .setOffset(offsetof(Vertex, color_));
    return attribute_descriptions;
}

}  // namespace odyssey