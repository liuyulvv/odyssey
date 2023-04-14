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
    m_deviceFuncs->vkDestroyShaderModule(m_window->device(), vertShaderModule, nullptr);
    m_deviceFuncs->vkDestroyShaderModule(m_window->device(), fragShaderModule, nullptr);
    m_deviceFuncs->vkDestroyPipeline(m_window->device(), graphicsPipeline, nullptr);
}

PipelineConfigInfo OdysseyPipeline::defaultPipelineConfigInfo(VkPrimitiveTopology primitiveTopology) {
    PipelineConfigInfo config{};

    config.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    config.viewportInfo.viewportCount = 1;
    config.viewportInfo.pViewports = nullptr;
    config.viewportInfo.scissorCount = 1;
    config.viewportInfo.pScissors = nullptr;

    config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config.inputAssemblyInfo.topology = primitiveTopology;
    config.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    config.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config.rasterizationInfo.depthClampEnable = VK_FALSE;
    config.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    config.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    config.rasterizationInfo.lineWidth = 1.0F;
    config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    config.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    config.rasterizationInfo.depthBiasEnable = VK_FALSE;
    config.rasterizationInfo.depthBiasConstantFactor = 0.0F;
    config.rasterizationInfo.depthBiasClamp = 0.0F;
    config.rasterizationInfo.depthBiasSlopeFactor = 0.0F;

    config.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config.multisampleInfo.sampleShadingEnable = VK_FALSE;
    config.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config.multisampleInfo.minSampleShading = 1.0F;
    config.multisampleInfo.pSampleMask = nullptr;
    config.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
    config.multisampleInfo.alphaToOneEnable = VK_FALSE;

    config.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    config.colorBlendAttachment.blendEnable = VK_FALSE;
    config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    config.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    config.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config.colorBlendInfo.logicOpEnable = VK_FALSE;
    config.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    config.colorBlendInfo.attachmentCount = 1;
    config.colorBlendInfo.pAttachments = &config.colorBlendAttachment;
    config.colorBlendInfo.blendConstants[0] = 0.0F;
    config.colorBlendInfo.blendConstants[1] = 0.0F;
    config.colorBlendInfo.blendConstants[2] = 0.0F;
    config.colorBlendInfo.blendConstants[3] = 0.0F;

    config.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config.depthStencilInfo.depthTestEnable = VK_TRUE;
    config.depthStencilInfo.depthWriteEnable = VK_TRUE;
    config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    config.depthStencilInfo.minDepthBounds = 0.0F;
    config.depthStencilInfo.maxDepthBounds = 1.0F;
    config.depthStencilInfo.stencilTestEnable = VK_FALSE;
    config.depthStencilInfo.front = {};
    config.depthStencilInfo.back = {};

    config.dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    config.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    config.dynamicStateInfo.pDynamicStates = config.dynamicStates.data();
    config.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(config.dynamicStates.size());
    config.dynamicStateInfo.flags = 0;
    return config;
}

void OdysseyPipeline::bind(const VkCommandBuffer& commandBuffer) {
    m_deviceFuncs->vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}

void OdysseyPipeline::createGraphicsPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, const PipelineConfigInfo& config) {
    createShaderModule(readFile(vertShaderPath), vertShaderModule);
    createShaderModule(readFile(fragShaderPath), fragShaderModule);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.module = fragShaderModule;
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
    pipelineInfo.pInputAssemblyState = &config.inputAssemblyInfo;
    pipelineInfo.pViewportState = &config.viewportInfo;
    pipelineInfo.pRasterizationState = &config.rasterizationInfo;
    pipelineInfo.pMultisampleState = &config.multisampleInfo;
    pipelineInfo.pColorBlendState = &config.colorBlendInfo;
    pipelineInfo.pDepthStencilState = &config.depthStencilInfo;
    pipelineInfo.pDynamicState = &config.dynamicStateInfo;
    pipelineInfo.layout = config.pipelineLayout;
    pipelineInfo.renderPass = config.renderPass;
    pipelineInfo.subpass = config.subpass;
    pipelineInfo.basePipelineIndex = -1;
    pipelineInfo.basePipelineHandle = nullptr;
    m_deviceFuncs->vkCreateGraphicsPipelines(m_window->device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
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