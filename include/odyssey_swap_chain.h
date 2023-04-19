/**
 * @file odyssey_swap_chain.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-10
 */

#if !defined(ODYSSEY_ODYSSEY_SWAP_CHAIN_H_)
#define ODYSSEY_ODYSSEY_SWAP_CHAIN_H_

#include <vector>

#include "odyssey_engine.h"

#if defined(_WIN32)
#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vulkan.hpp"

namespace odyssey {

class OdysseySwapChain {
public:
    OdysseySwapChain(OdysseyEngine* engine, int width, int height);
    ~OdysseySwapChain();

    OdysseySwapChain() = delete;
    OdysseySwapChain(const OdysseySwapChain& odysseySwapChain) = delete;
    OdysseySwapChain(OdysseySwapChain&& odysseySwapChain) = delete;
    OdysseySwapChain& operator=(const OdysseySwapChain& odysseySwapChain) = delete;
    OdysseySwapChain& operator=(OdysseySwapChain&& odysseySwapChain) = delete;

public:
    const vk::Format& getSwapChainImageFormat() const;
    size_t getImageCount() const;
    vk::Extent2D getSwapChainExtent() const;
    const vk::Framebuffer& getFrameBuffer(size_t index) const;
    const vk::ImageView& getImageView(size_t index) const;
    const vk::RenderPass& getRenderPass() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    float getExtentAspectRatio() const;
    uint32_t acquireNextImage();
    void submitCommandBuffers(const vk::CommandBuffer& buffers, uint32_t imageIndex);

private:
    void createSwapChain();
    void createRenderPass();
    void createDepthResources();
    void createFrameBuffers();
    void createSyncObjects();

private:
    static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
    vk::Format findDepthFormat() const;

public:
    static constexpr int MAX_FRAMES_IN_FLIGHT{2};

private:
    OdysseyEngine* m_engine{nullptr};
    vk::Extent2D m_windowExtent{};
    vk::Format m_swapChainImageFormat{};
    vk::Extent2D m_swapChainExtent{};
    vk::SwapchainKHR m_swapChain{};
    std::vector<vk::Image> m_swapChainImages{};
    std::vector<vk::ImageView> m_swapChainImageViews{};
    vk::RenderPass m_renderPass{};
    std::vector<vk::Image> m_depthImages{};
    std::vector<vk::DeviceMemory> m_depthImageMemories{};
    std::vector<vk::ImageView> m_depthImageViews{};
    std::vector<vk::Framebuffer> m_swapChainFrameBuffers{};
    std::vector<vk::Semaphore> m_imageAvailableSemaphores{};
    std::vector<vk::Semaphore> m_renderFinishedSemaphores{};
    std::vector<vk::Fence> m_inFlightFences{};
    std::vector<vk::Fence> m_imagesInFlight{};
    size_t m_currentFrame{0};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_SWAP_CHAIN_H_