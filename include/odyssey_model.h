#pragma once

/**
 * @file odyssey_model.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-11
 */

#include <memory>
#include <string>
#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "odyssey_header.h"

namespace odyssey {

class OdysseyDevice;

class OdysseyModel {
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 normal;
        glm::vec2 uv;
        bool operator==(const Vertex& other) const;
        static std::vector<vk::VertexInputBindingDescription> getBindingDescriptions();
        static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
    };

    struct Builder {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};
        void loadModel(const std::string& filepath);

    private:
        void processNode(const aiNode* node, const aiScene* scene);
        void processMesh(const aiMesh* mesh);
    };

public:
    OdysseyModel(OdysseyDevice* device, const Builder& builder);
    ~OdysseyModel();

    OdysseyModel() = delete;
    OdysseyModel(const OdysseyModel& odysseyModel) = delete;
    OdysseyModel(OdysseyModel&& odysseyModel) = delete;
    OdysseyModel& operator=(const OdysseyModel& odysseyModel) = delete;
    OdysseyModel& operator=(OdysseyModel&& odysseyModel) = delete;

public:
    static std::shared_ptr<OdysseyModel> createModelFromFile(OdysseyDevice* device, const std::string& filepath);

public:
    void bind(vk::CommandBuffer& commandBuffer) const;
    void draw(vk::CommandBuffer& commandBuffer) const;

private:
    void createVertexBuffer(const std::vector<Vertex>& vertices);
    void createIndexBuffer(const std::vector<uint32_t>& indices);

private:
    OdysseyDevice* m_device{};
    vk::Buffer m_vertexBuffer{};
    vk::DeviceMemory m_vertexBufferMemory{};
    uint32_t m_vertexCount{0};
    bool m_hasIndexBuffer{false};
    vk::Buffer m_indexBuffer{};
    vk::DeviceMemory m_indexBufferMemory{};
    uint32_t m_indexCount{0};
};

}  // namespace odyssey

namespace std {
template <>
struct hash<odyssey::OdysseyModel::Vertex> {
    size_t operator()(const odyssey::OdysseyModel::Vertex& vertex) const {
        return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec3>()(vertex.normal) << 1) ^ (hash<glm::vec2>()(vertex.uv) << 1);
    }
};
}  // namespace std