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

#include <vector>

#include "glm/glm.hpp"
#include "odyssey_engine.h"
#include "vulkan/vulkan.hpp"

namespace odyssey {

class OdysseyModel {
public:
    struct Vertex {
        glm::vec2 position_;
        static std::vector<vk::VertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions();
    };

public:
    OdysseyModel(OdysseyEngine& engine, const std::vector<Vertex>& vertices);
    ~OdysseyModel();

    OdysseyModel() = delete;
    OdysseyModel(const OdysseyModel& odyssey_model) = delete;
    OdysseyModel(OdysseyModel&& odyssey_model) = delete;
    OdysseyModel& operator=(const OdysseyModel& odyssey_model) = delete;
    OdysseyModel& operator=(OdysseyModel&& odyssey_model) = delete;

public:
    void Bind(vk::CommandBuffer& command_buffer) const;
    void Draw(vk::CommandBuffer& command_buffer) const;

private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);

private:
    OdysseyEngine& engine_;
    vk::Buffer vertex_buffer_{};
    vk::DeviceMemory vertex_buffer_memory_{};
    uint32_t vertex_count_{0};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_MODEL_H_