#pragma once

/**
 * @file odyssey_model.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-11
 */

#if !defined(GLM_FORCE_RADIANS)
#define GLM_FORCE_RADIANS
#endif  // GLM_FORCE_RADIANS

#if !defined(GLM_DEPTH_ZERO_TO_ONE)
#define GLM_DEPTH_ZERO_TO_ONE
#endif  // GLM_DEPTH_ZERO_TO_ONE

#include <memory>
#include <vector>

#include "glm/glm.hpp"

#if defined(_WIN32)
#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vulkan.hpp"

namespace odyssey {

class OdysseyEngine;

class OdysseyModel {
public:
    struct Vertex {
        glm::vec2 position;
        glm::vec4 color;
        static std::vector<vk::VertexInputBindingDescription> getBindingDescriptions();
        static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
    };

public:
    OdysseyModel(OdysseyEngine* engine, const std::vector<Vertex>& vertices);
    ~OdysseyModel();

    OdysseyModel() = delete;
    OdysseyModel(const OdysseyModel& odysseyModel) = delete;
    OdysseyModel(OdysseyModel&& odysseyModel) = delete;
    OdysseyModel& operator=(const OdysseyModel& odysseyModel) = delete;
    OdysseyModel& operator=(OdysseyModel&& odysseyModel) = delete;

public:
    void bind(vk::CommandBuffer& commandBuffer) const;
    void draw(vk::CommandBuffer& commandBuffer) const;

private:
    void createVertexBuffer(const std::vector<Vertex>& vertices);

private:
    OdysseyEngine* m_engine{nullptr};
    vk::Buffer m_vertexBuffer{};
    vk::DeviceMemory m_vertexBufferMemory{};
    uint32_t m_vertexCount{0};
};

}  // namespace odyssey