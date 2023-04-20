#pragma once

/**
 * @file odyssey_pipeline.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

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

    vk::PipelineViewportStateCreateInfo viewportInfo{};
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    vk::PipelineRasterizationStateCreateInfo rasterizationInfo{};
    vk::PipelineMultisampleStateCreateInfo multisampleInfo{};
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    vk::PipelineColorBlendStateCreateInfo colorBlendInfo{};
    vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{};
    std::vector<vk::DynamicState> dynamicStates{};
    vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
    vk::PipelineLayout pipelineLayout{nullptr};
    vk::RenderPass renderPass{nullptr};
    uint32_t subpass{0};
};

class OdysseyPipeline {
public:
    OdysseyPipeline(OdysseyEngine* engine, const std::string& vertShaderPath, const std::string& fragShaderPath, const PipelineConfigInfo& config);
    ~OdysseyPipeline();

    OdysseyPipeline() = delete;
    OdysseyPipeline(const OdysseyPipeline& odysseyPipeline) = delete;
    OdysseyPipeline(OdysseyPipeline&& odysseyPipeline) = delete;
    OdysseyPipeline& operator=(const OdysseyPipeline& odysseyPipeline) = delete;
    OdysseyPipeline& operator=(OdysseyPipeline&& odysseyPipeline) = delete;

public:
    static PipelineConfigInfo defaultPipelineConfigInfo(vk::PrimitiveTopology primitiveTopology = vk::PrimitiveTopology::eTriangleList, float lineWidth = 1.0F);
    void bind(const vk::CommandBuffer& buffer);

private:
    void createGraphicsPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, const PipelineConfigInfo& config);
    static std::vector<char> readFile(const std::string& path);
    vk::ShaderModule createShaderModule(const std::vector<char>& code);

private:
    OdysseyEngine* m_engine;
    vk::Pipeline m_graphicsPipeline{};
    vk::ShaderModule vertShaderModule{};
    vk::ShaderModule fragShaderModule{};
};

}  // namespace odyssey