/**
 * @file odyssey_window_render.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_WINDOW_RENDER_H_)
#define ODYSSEY_WINDOW_RENDER_H_

#include <vulkan/vulkan.h>

#include <QVulkanDeviceFunctions>
#include <QVulkanWindowRenderer>
#include <string>
#include <vector>

#include "odyssey_pipeline.h"

namespace odyssey {

class OdysseyWindow;

class OdysseyWindowRender : public QVulkanWindowRenderer {
public:
    explicit OdysseyWindowRender(OdysseyWindow* window);
    ~OdysseyWindowRender() = default;

    OdysseyWindowRender() = delete;
    OdysseyWindowRender(const OdysseyWindowRender& odysseyWindowRender) = delete;
    OdysseyWindowRender(OdysseyWindowRender&& odysseyWindowRender) = delete;
    OdysseyWindowRender& operator=(const OdysseyWindowRender& odysseyWindowRender) = delete;
    OdysseyWindowRender& operator=(OdysseyWindowRender&& odysseyWindowRender) = delete;

public:
    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;
    void startNextFrame() override;

public:
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* memory);
    void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

private:
    void createPipelineLayout();
    void createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, VkPrimitiveTopology primitiveTopology);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

private:
    OdysseyWindow* m_window{};
    QVulkanDeviceFunctions* m_deviceFuncs{};
    VkPipelineLayout m_pipelineLayout{VK_NULL_HANDLE};
    std::vector<OdysseyPipeline*> m_pipelines{};
};

}  // namespace odyssey

#endif  // ODYSSEY_WINDOW_RENDER_H_