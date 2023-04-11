/**
 * @file odyssey_pipeline.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_PIPELINE_H_)
#define ODYSSEY_ODYSSEY_PIPELINE_H_

#include <string>
#include <vector>

#include "odyssey_engine.h"
#include "vulkan/vulkan.hpp"

namespace odyssey {

struct PipelineConfigInfo {
    vk::Viewport view_port_{};
    vk::Rect2D scissor_{};
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_info_{};
    vk::PipelineRasterizationStateCreateInfo rasterization_info_{};
    vk::PipelineMultisampleStateCreateInfo multisample_info_{};
    vk::PipelineColorBlendAttachmentState color_blend_attachment_{};
    vk::PipelineColorBlendStateCreateInfo color_blend_info_{};
    vk::PipelineDepthStencilStateCreateInfo depth_stencil_info_{};
    vk::PipelineDynamicStateCreateInfo dynamic_state_info_{};
    vk::PipelineLayout pipeline_layout_{nullptr};
    vk::RenderPass render_pass_{nullptr};
    uint32_t subpass_{0};
};

class OdysseyPipeline {
public:
    OdysseyPipeline(OdysseyEngine& engine, const std::string& vertex_shader_path, const std::string& fragment_shader_path, const PipelineConfigInfo& config);
    ~OdysseyPipeline();

    OdysseyPipeline() = delete;
    OdysseyPipeline(const OdysseyPipeline& odyssey_pipeline) = delete;
    OdysseyPipeline(OdysseyPipeline&& odyssey_pipeline) = delete;
    OdysseyPipeline& operator=(const OdysseyPipeline& odyssey_pipeline) = delete;
    OdysseyPipeline& operator=(OdysseyPipeline&& odyssey_pipeline) = delete;

public:
    static PipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);
    void Bind(const vk::CommandBuffer& buffer);

private:
    void CreateGraphicsPipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path, const PipelineConfigInfo& config);
    static std::vector<char> ReadFile(const std::string& path);
    vk::ShaderModule CreateShaderModule(const std::vector<char>& code);

private:
    OdysseyEngine& engine_;
    vk::Pipeline graphics_pipeline_{};
    vk::ShaderModule vert_shader_module_{};
    vk::ShaderModule frag_shader_module_{};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_PIPELINE_H_