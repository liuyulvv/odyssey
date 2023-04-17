/**
 * @file odyssey_pipeline.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_pipeline.h"

#include <odyssey_window.h>

#include <cstring>
#include <fstream>
#include <stdexcept>

#include "odyssey_model.h"

namespace odyssey {

OdysseyPipeline::OdysseyPipeline(OdysseyWindow* window, QVulkanDeviceFunctions* deviceFuncs, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const PipelineConfigInfo& config) : m_window(window), m_deviceFuncs(deviceFuncs) {
    createGraphicsPipeline(vertexShaderPath, fragmentShaderPath, config);
}

OdysseyPipeline::~OdysseyPipeline() {
    m_deviceFuncs->vkDestroyShaderModule(m_window->device(), m_vertShaderModule, nullptr);
    m_deviceFuncs->vkDestroyShaderModule(m_window->device(), m_fragShaderModule, nullptr);
    m_deviceFuncs->vkDestroyPipeline(m_window->device(), m_graphicsPipeline, nullptr);
}

PipelineConfigInfo OdysseyPipeline::defaultPipelineConfigInfo(VkPrimitiveTopology primitiveTopology) {
    PipelineConfigInfo config{};

    config.m_viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    config.m_viewportInfo.viewportCount = 1;
    config.m_viewportInfo.pViewports = nullptr;
    config.m_viewportInfo.scissorCount = 1;
    config.m_viewportInfo.pScissors = nullptr;

    config.m_inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config.m_inputAssemblyInfo.topology = primitiveTopology;
    config.m_inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    config.m_rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config.m_rasterizationInfo.depthClampEnable = VK_FALSE;
    config.m_rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    config.m_rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    config.m_rasterizationInfo.lineWidth = 1.0F;
    config.m_rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    config.m_rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    config.m_rasterizationInfo.depthBiasEnable = VK_FALSE;
    config.m_rasterizationInfo.depthBiasConstantFactor = 0.0F;
    config.m_rasterizationInfo.depthBiasClamp = 0.0F;
    config.m_rasterizationInfo.depthBiasSlopeFactor = 0.0F;

    config.m_multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config.m_multisampleInfo.sampleShadingEnable = VK_FALSE;
    config.m_multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config.m_multisampleInfo.minSampleShading = 1.0F;
    config.m_multisampleInfo.pSampleMask = nullptr;
    config.m_multisampleInfo.alphaToCoverageEnable = VK_FALSE;
    config.m_multisampleInfo.alphaToOneEnable = VK_FALSE;

    config.m_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    config.m_colorBlendAttachment.blendEnable = VK_FALSE;
    config.m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    config.m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    config.m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    config.m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    config.m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    config.m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    config.m_colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config.m_colorBlendInfo.logicOpEnable = VK_FALSE;
    config.m_colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    config.m_colorBlendInfo.attachmentCount = 1;
    config.m_colorBlendInfo.pAttachments = &config.m_colorBlendAttachment;
    config.m_colorBlendInfo.blendConstants[0] = 0.0F;
    config.m_colorBlendInfo.blendConstants[1] = 0.0F;
    config.m_colorBlendInfo.blendConstants[2] = 0.0F;
    config.m_colorBlendInfo.blendConstants[3] = 0.0F;

    config.m_depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config.m_depthStencilInfo.depthTestEnable = VK_TRUE;
    config.m_depthStencilInfo.depthWriteEnable = VK_TRUE;
    config.m_depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    config.m_depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    config.m_depthStencilInfo.minDepthBounds = 0.0F;
    config.m_depthStencilInfo.maxDepthBounds = 1.0F;
    config.m_depthStencilInfo.stencilTestEnable = VK_FALSE;
    config.m_depthStencilInfo.front = {};
    config.m_depthStencilInfo.back = {};

    config.m_dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    config.m_dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    config.m_dynamicStateInfo.pDynamicStates = config.m_dynamicStates.data();
    config.m_dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(config.m_dynamicStates.size());
    config.m_dynamicStateInfo.flags = 0;
    return config;
}

void OdysseyPipeline::bind(const VkCommandBuffer& commandBuffer) {
    m_deviceFuncs->vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
}

void OdysseyPipeline::createGraphicsPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, const PipelineConfigInfo& config) {
    createShaderModule(readFile(vertShaderPath), &m_vertShaderModule);
    createShaderModule(readFile(fragShaderPath), &m_fragShaderModule);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.module = m_vertShaderModule;
    vertShaderStageInfo.pName = "main";
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.module = m_fragShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    auto bindingDescriptions = OdysseyModel::Vertex::getBindingDescriptions();
    auto attributeDescriptions = OdysseyModel::Vertex::getAttributeDescriptions();
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{vertShaderStageInfo, fragShaderStageInfo};

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &config.m_inputAssemblyInfo;
    pipelineInfo.pViewportState = &config.m_viewportInfo;
    pipelineInfo.pRasterizationState = &config.m_rasterizationInfo;
    pipelineInfo.pMultisampleState = &config.m_multisampleInfo;
    pipelineInfo.pColorBlendState = &config.m_colorBlendInfo;
    pipelineInfo.pDepthStencilState = &config.m_depthStencilInfo;
    pipelineInfo.pDynamicState = &config.m_dynamicStateInfo;
    pipelineInfo.layout = config.m_pipelineLayout;
    pipelineInfo.renderPass = config.m_renderPass;
    pipelineInfo.subpass = config.m_subpass;
    pipelineInfo.basePipelineIndex = -1;
    pipelineInfo.basePipelineHandle = nullptr;
    auto err = m_deviceFuncs->vkCreateGraphicsPipelines(m_window->device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline);
}

std::vector<char> OdysseyPipeline::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path + "");
    }
    auto fileSize = static_cast<std::streamsize>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

void OdysseyPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    m_deviceFuncs->vkCreateShaderModule(m_window->device(), &createInfo, nullptr, shaderModule);
}

}  // namespace odyssey