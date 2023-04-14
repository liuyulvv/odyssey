/**
 * @file odyssey_model.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-11
 */

#if !defined(ODYSSEY_ODYSSEY_MODEL_H_)
#define ODYSSEY_ODYSSEY_MODEL_H_

#if !defined(GLM_FORCE_RADIANS)
#define GLM_FORCE_RADIANS
#endif  // GLM_FORCE_RADIANS

#if !defined(GLM_DEPTH_ZERO_TO_ONE)
#define GLM_DEPTH_ZERO_TO_ONE
#endif  // GLM_DEPTH_ZERO_TO_ONE

#include <vulkan/vulkan.h>

#include <QVulkanDeviceFunctions>
#include <glm/glm.hpp>
#include <vector>

namespace odyssey {

class OdysseyWindow;

class OdysseyModel {
public:
    struct Vertex {
        glm::vec2 position;
        glm::vec4 color;
        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

public:
    OdysseyModel(OdysseyWindow* window, QVulkanDeviceFunctions* deviceFuncs, const std::vector<Vertex>& vertices);
    ~OdysseyModel();

    OdysseyModel() = delete;
    OdysseyModel(const OdysseyModel& odyssey_model) = delete;
    OdysseyModel(OdysseyModel&& odyssey_model) = delete;
    OdysseyModel& operator=(const OdysseyModel& odyssey_model) = delete;
    OdysseyModel& operator=(OdysseyModel&& odyssey_model) = delete;

public:
    void bind(VkCommandBuffer& command_buffer) const;
    void draw(VkCommandBuffer& command_buffer) const;

private:
    void createVertexBuffer(const std::vector<Vertex>& vertices);

private:
    OdysseyWindow* m_window;
    QVulkanDeviceFunctions* m_deviceFuncs;
    VkBuffer vertexBuffer{};
    VkDeviceMemory vertexBufferMemory{};
    uint32_t vertexCount{0};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_MODEL_H_