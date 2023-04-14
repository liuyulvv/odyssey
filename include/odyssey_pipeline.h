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

    VkPipelineViewportStateCreateInfo viewportInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    std::vector<VkDynamicState> dynamicStates{};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    VkPipelineLayout pipelineLayout{nullptr};
    VkRenderPass renderPass{nullptr};
    uint32_t subpass{0};
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
    void createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule);

private:
    OdysseyWindow* m_window{};
    QVulkanDeviceFunctions* m_deviceFuncs{};
    VkPipeline graphicsPipeline{};
    VkShaderModule vertShaderModule{};
    VkShaderModule fragShaderModule{};
};

}  // namespace odyssey

#endif  // odysseyPipeline_H_