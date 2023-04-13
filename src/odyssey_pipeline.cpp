/**
 * @file odyssey_pipeline.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_pipeline.h"

#include <cstring>
#include <fstream>
#include <stdexcept>

#include "fmt/format.h"
#include "odyssey_model.h"

namespace odyssey {

OdysseyPipeline::OdysseyPipeline(std::shared_ptr<OdysseyEngine> engine, const std::string& vertex_shader_path, const std::string& fragment_shader_path, const PipelineConfigInfo& config) : engine_(std::move(engine)) {
    CreateGraphicsPipeline(vertex_shader_path, fragment_shader_path, config);
}

OdysseyPipeline::~OdysseyPipeline() {
    engine_->Device().destroyShaderModule(vert_shader_module_);
    engine_->Device().destroyShaderModule(frag_shader_module_);
    engine_->Device().destroyPipeline(graphics_pipeline_);
}

PipelineConfigInfo OdysseyPipeline::DefaultPipelineConfigInfo(vk::PrimitiveTopology primitive_topology, float line_width) {
    PipelineConfigInfo config{};

    config.viewport_info_
        .setViewportCount(1)
        .setPViewports(nullptr)
        .setScissorCount(1)
        .setPScissors(nullptr);

    config.input_assembly_info_
        .setTopology(primitive_topology)
        .setPrimitiveRestartEnable(false);

    config.rasterization_info_
        .setDepthClampEnable(false)
        .setRasterizerDiscardEnable(false)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(line_width)
        .setCullMode(vk::CullModeFlagBits::eNone)
        .setFrontFace(vk::FrontFace::eClockwise)
        .setDepthBiasEnable(false)
        .setDepthBiasConstantFactor(0.0F)
        .setDepthBiasClamp(0.0F)
        .setDepthBiasSlopeFactor(0.0F);

    config.multisample_info_
        .setSampleShadingEnable(false)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1)
        .setMinSampleShading(1.0F)
        .setPSampleMask(nullptr)
        .setAlphaToCoverageEnable(false)
        .setAlphaToOneEnable(false);

    config.color_blend_attachment_
        .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
        .setBlendEnable(false)
        .setSrcColorBlendFactor(vk::BlendFactor::eOne)
        .setDstColorBlendFactor(vk::BlendFactor::eZero)
        .setColorBlendOp(vk::BlendOp::eAdd)
        .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
        .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
        .setAlphaBlendOp(vk::BlendOp::eAdd);

    config.color_blend_info_
        .setLogicOpEnable(false)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachmentCount(1)
        .setAttachments(config.color_blend_attachment_)
        .setBlendConstants({0.0F, 0.0F, 0.0F, 0.0F});

    config.depth_stencil_info_
        .setDepthTestEnable(true)
        .setDepthWriteEnable(true)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setDepthBoundsTestEnable(false)
        .setMinDepthBounds(0.0F)
        .setMaxDepthBounds(1.0F)
        .setStencilTestEnable(false)
        .setFront({})
        .setBack({});

    config.dynamic_states_ = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    config.dynamic_state_info_
        .setDynamicStateCount(static_cast<uint32_t>(config.dynamic_states_.size()))
        .setDynamicStates(config.dynamic_states_);

    return config;
}

void OdysseyPipeline::Bind(const vk::CommandBuffer& buffer) {
    buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline_);
}

void OdysseyPipeline::CreateGraphicsPipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path, const PipelineConfigInfo& config) {
    auto vert_shader_code = ReadFile(vertex_shader_path);
    auto frag_shader_code = ReadFile(fragment_shader_path);
    vert_shader_module_ = CreateShaderModule(vert_shader_code);
    frag_shader_module_ = CreateShaderModule(frag_shader_code);

    vk::PipelineShaderStageCreateInfo vert_shader_stage_info;
    vert_shader_stage_info
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(vert_shader_module_)
        .setPName("main");

    vk::PipelineShaderStageCreateInfo frag_shader_stage_info;
    frag_shader_stage_info
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(frag_shader_module_)
        .setPName("main");

    vk::PipelineVertexInputStateCreateInfo vertex_input_info;
    auto binding_descriptions = OdysseyModel::Vertex::GetBindingDescriptions();
    auto attribute_descriptions = OdysseyModel::Vertex::GetAttributeDescriptions();
    vertex_input_info
        .setVertexBindingDescriptionCount(static_cast<uint32_t>(binding_descriptions.size()))
        .setVertexBindingDescriptions(binding_descriptions)
        .setVertexAttributeDescriptionCount(static_cast<uint32_t>(attribute_descriptions.size()))
        .setVertexAttributeDescriptions(attribute_descriptions);

    std::array<vk::PipelineShaderStageCreateInfo, 2> shader_stages{vert_shader_stage_info, frag_shader_stage_info};

    vk::GraphicsPipelineCreateInfo pipeline_info;
    pipeline_info
        .setStageCount(static_cast<uint32_t>(shader_stages.size()))
        .setStages(shader_stages)
        .setPVertexInputState(&vertex_input_info)
        .setPInputAssemblyState(&config.input_assembly_info_)
        .setPViewportState(&config.viewport_info_)
        .setPRasterizationState(&config.rasterization_info_)
        .setPMultisampleState(&config.multisample_info_)
        .setPColorBlendState(&config.color_blend_info_)
        .setPDepthStencilState(&config.depth_stencil_info_)
        .setPDynamicState(&config.dynamic_state_info_)
        .setLayout(config.pipeline_layout_)
        .setRenderPass(config.render_pass_)
        .setSubpass(config.subpass_)
        .setBasePipelineIndex(-1)
        .setBasePipelineHandle(nullptr);
    graphics_pipeline_ = engine_->Device().createGraphicsPipeline(nullptr, pipeline_info).value;
}

std::vector<char> OdysseyPipeline::ReadFile(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open file: {}.", path));
    }
    auto file_size = static_cast<std::streamsize>(file.tellg());
    std::vector<char> buffer(file_size);
    file.seekg(0);
    file.read(buffer.data(), file_size);
    file.close();
    return buffer;
}

vk::ShaderModule OdysseyPipeline::CreateShaderModule(const std::vector<char>& code) {
    vk::ShaderModuleCreateInfo create_info;
    create_info.setCodeSize(code.size());
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());
    return engine_->Device().createShaderModule(create_info);
}

}  // namespace odyssey