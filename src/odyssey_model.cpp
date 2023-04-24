/**
 * @file odyssey_model.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-11
 */

#include "odyssey_model.h"

#include <array>
#include <cstddef>
#include <cstring>
#include <unordered_map>

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

std::shared_ptr<OdysseyModel> OdysseyModel::createModelFromFile(OdysseyDevice* device, const std::string& filepath) {
    Builder builder{};
    builder.loadModel(filepath);
    return std::make_shared<OdysseyModel>(device, builder);
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

bool OdysseyModel::Vertex::operator==(const Vertex& other) const {
    return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
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

void OdysseyModel::Builder::loadModel(const std::string& filepath) {
    Assimp::Importer importer;
    const auto* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
    processNode(scene->mRootNode, scene);
}

void OdysseyModel::Builder::processNode(const aiNode* node, const aiScene* scene) {
    for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
        auto* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh);
    }
    for (uint32_t i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

void OdysseyModel::Builder::processMesh(const aiMesh* mesh) {
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
        glm::vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;

        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;

        glm::vec3 color;
        if (mesh->mColors[0]) {
            color.x = mesh->mColors[0][i].r;
            color.y = mesh->mColors[0][i].g;
            color.z = mesh->mColors[0][i].b;
        } else {
            color = {1.0F, 1.0F, 1.0F};
        }

        glm::vec2 uv;
        if (mesh->mTextureCoords[0]) {
            uv.x = mesh->mTextureCoords[0][i].x;
            uv.y = mesh->mTextureCoords[0][i].y;
        } else {
            uv = {0.0F, 0.0F};
        }

        Vertex vertex{};
        vertex.position = position;
        vertex.color = color;
        vertex.normal = normal;
        vertex.uv = uv;
        vertices.push_back(vertex);

        if (!uniqueVertices.contains(vertex)) {
            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
            vertices.push_back(vertex);
        }
        indices.push_back(uniqueVertices[vertex]);
    }
}

}  // namespace odyssey