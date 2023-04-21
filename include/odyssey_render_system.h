#pragma once

/**
 * @file odyssey_render_system.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-21
 */

#include <memory>
#include <vector>

#include "odyssey_header.h"
#include "odyssey_object.h"
#include "odyssey_pipeline.h"

namespace odyssey {

class OdysseyDevice;

struct PushConstantData {
    glm::mat4 transform{1.F};
    alignas(16) glm::vec4 color;
};

class OdysseyRenderSystem {
public:
    OdysseyRenderSystem(OdysseyDevice* device, vk::RenderPass renderPass);
    ~OdysseyRenderSystem();
    OdysseyRenderSystem() = delete;
    OdysseyRenderSystem(const OdysseyRenderSystem& odysseyRenderSystem) = default;
    OdysseyRenderSystem(OdysseyRenderSystem&& odysseyRenderSystem) = default;
    OdysseyRenderSystem& operator=(const OdysseyRenderSystem& odysseyRenderSystem) = default;
    OdysseyRenderSystem& operator=(OdysseyRenderSystem&& odysseyRenderSystem) = default;

public:
    void renderObjects(vk::CommandBuffer commandBuffer, std::vector<OdysseyObject>& objects);

private:
    void createPipelineLayout();
    std::unique_ptr<OdysseyPipeline> createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, vk::PrimitiveTopology primitiveTopology, float lineWidth, vk::RenderPass renderPass);

private:
    OdysseyDevice* m_device;
    vk::PipelineLayout m_pipelineLayout{};
    std::unique_ptr<OdysseyPipeline> m_pipelineLine{};
};

}  // namespace odyssey