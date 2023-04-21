#pragma once

/**
 * @file odyssey_render.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-21
 */

#include <memory>
#include <vector>

#include "odyssey_device.h"
#include "odyssey_header.h"
#include "odyssey_window.h"

namespace odyssey {

class OdysseyRender {
public:
    explicit OdysseyRender(OdysseyWindow* window, OdysseyDevice* device);
    ~OdysseyRender();

    OdysseyRender() = delete;
    OdysseyRender(const OdysseyRender& odysseyRender) = delete;
    OdysseyRender(OdysseyRender&& odysseyRender) = delete;
    OdysseyRender& operator=(const OdysseyRender& odysseyRender) = delete;
    OdysseyRender& operator=(OdysseyRender&& odysseyRender) = delete;

public:
    const vk::RenderPass& getSwapChainRenderPass() const;
    bool isFrameInProgress() const;
    vk::CommandBuffer getCurrentCommandBuffer() const;

public:
    vk::CommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(vk::CommandBuffer commandBuffer);
    void endSwapChainRenderPass(vk::CommandBuffer commandBuffer);

private:
    void recreateSwapChain();
    void createCommandBuffers();
    void freeCommandBuffers();

private:
    OdysseyWindow* m_window{};
    OdysseyDevice* m_device{};
    std::vector<vk::CommandBuffer> m_commandBuffers{};
    std::unique_ptr<OdysseySwapChain> m_swapChain{};
    uint32_t m_currentImageIndex{};
    bool m_isFrameStarted{false};
};

}  // namespace odyssey