/**
 * @file odysseyPipeline.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(odysseyPipeline_H_)
#define odysseyPipeline_H_

#include <vulkan/vulkan.h>

#include <QVulkanDeviceFunctions>
#include <string>
#include <vector>

namespace odyssey {

class OdysseyWindow;

struct PipelineConfigInfo {
    PipelineConfigInfo() = default;

    VkPipelineViewportStateCreateInfo m_viewportInfo{};
    VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyInfo{};
    VkPipelineRasterizationStateCreateInfo m_rasterizationInfo{};
    VkPipelineMultisampleStateCreateInfo m_multisampleInfo{};
    VkPipelineColorBlendAttachmentState m_colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo m_colorBlendInfo{};
    VkPipelineDepthStencilStateCreateInfo m_depthStencilInfo{};
    std::vector<VkDynamicState> m_dynamicStates{};
    VkPipelineDynamicStateCreateInfo m_dynamicStateInfo{};
    VkPipelineLayout m_pipelineLayout{nullptr};
    VkRenderPass m_renderPass{nullptr};
    uint32_t m_subpass{0};
};

class OdysseyPipeline {
public:
    OdysseyPipeline(OdysseyWindow* window, QVulkanDeviceFunctions* deviceFuncs, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const PipelineConfigInfo& config);
    ~OdysseyPipeline();

    OdysseyPipeline() = delete;
    OdysseyPipeline(const OdysseyPipeline& odysseyPipeline) = delete;
    OdysseyPipeline(OdysseyPipeline&& odysseyPipeline) = delete;
    OdysseyPipeline& operator=(const OdysseyPipeline& odysseyPipeline) = delete;
    OdysseyPipeline& operator=(OdysseyPipeline&& odysseyPipeline) = delete;

public:
    static PipelineConfigInfo defaultPipelineConfigInfo(VkPrimitiveTopology primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    void bind(const VkCommandBuffer& buffer);

private:
    void createGraphicsPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, const PipelineConfigInfo& config);
    static std::vector<char> readFile(const std::string& path);
    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

private:
    OdysseyWindow* m_window{};
    QVulkanDeviceFunctions* m_deviceFuncs{};
    VkPipeline m_graphicsPipeline{};
    VkShaderModule m_vertShaderModule{};
    VkShaderModule m_fragShaderModule{};
};

}  // namespace odyssey

#endif  // odysseyPipeline_H_