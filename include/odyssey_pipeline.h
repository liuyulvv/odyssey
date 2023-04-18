/**
 * @file odyssey_pipeline.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_PIPELINE_H_)
#define ODYSSEY_ODYSSEY_PIPELINE_H_

#include <memory>
#include <string>
#include <vector>

#include "odyssey_engine.h"

#if defined(_WIN32)
#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vulkan.hpp"

namespace odyssey {

struct PipelineConfigInfo {
    PipelineConfigInfo() = default;

    vk::PipelineViewportStateCreateInfo viewport_info_{};
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_info_{};
    vk::PipelineRasterizationStateCreateInfo rasterization_info_{};
    vk::PipelineMultisampleStateCreateInfo multisample_info_{};
    vk::PipelineColorBlendAttachmentState color_blend_attachment_{};
    vk::PipelineColorBlendStateCreateInfo color_blend_info_{};
    vk::PipelineDepthStencilStateCreateInfo depth_stencil_info_{};
    std::vector<vk::DynamicState> dynamic_states_{};
    vk::PipelineDynamicStateCreateInfo dynamic_state_info_{};
    vk::PipelineLayout pipeline_layout_{nullptr};
    vk::RenderPass render_pass_{nullptr};
    uint32_t subpass_{0};
};

class OdysseyPipeline {
public:
    OdysseyPipeline(OdysseyEngine* engine, const std::string& vertex_shader_path, const std::string& fragment_shader_path, const PipelineConfigInfo& config);
    ~OdysseyPipeline();

    OdysseyPipeline() = delete;
    OdysseyPipeline(const OdysseyPipeline& odyssey_pipeline) = delete;
    OdysseyPipeline(OdysseyPipeline&& odyssey_pipeline) = delete;
    OdysseyPipeline& operator=(const OdysseyPipeline& odyssey_pipeline) = delete;
    OdysseyPipeline& operator=(OdysseyPipeline&& odyssey_pipeline) = delete;

public:
    static PipelineConfigInfo DefaultPipelineConfigInfo(vk::PrimitiveTopology primitive_topology = vk::PrimitiveTopology::eTriangleList, float line_width = 1.0F);
    void Bind(const vk::CommandBuffer& buffer);

private:
    void CreateGraphicsPipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path, const PipelineConfigInfo& config);
    static std::vector<char> ReadFile(const std::string& path);
    vk::ShaderModule CreateShaderModule(const std::vector<char>& code);

private:
    OdysseyEngine* engine_;
    vk::Pipeline graphics_pipeline_{};
    vk::ShaderModule vert_shader_module_{};
    vk::ShaderModule frag_shader_module_{};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_PIPELINE_H_