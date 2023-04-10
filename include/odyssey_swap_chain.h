/**
 * @file odyssey_swap_chain.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-10
 */

#if !defined(ODYSSEY_ODYSSEY_SWAP_CHAIN_H_)
#define ODYSSEY_ODYSSEY_SWAP_CHAIN_H_

#include <vector>

#include "odyssey_engine.h"
#include "vulkan/vulkan.hpp"

namespace odyssey {

class OdysseySwapChain {
public:
    OdysseySwapChain(OdysseyEngine& engine, vk::Extent2D window_extent);
    ~OdysseySwapChain();

    OdysseySwapChain() = delete;
    OdysseySwapChain(const OdysseySwapChain& odyssey_swap_chain) = delete;
    OdysseySwapChain(OdysseySwapChain&& odyssey_swap_chain) = delete;
    OdysseySwapChain& operator=(const OdysseySwapChain& odyssey_swap_chain) = delete;
    OdysseySwapChain& operator=(OdysseySwapChain&& odyssey_swap_chain) = delete;

public:
    const vk::Format& GetSwapChainImageFormat() const;
    size_t GetImageCount() const;
    vk::Extent2D GetSwapChainExtent() const;
    const vk::Framebuffer& GetFrameBuffer(int index) const;
    const vk::ImageView& GetImageView(int index) const;
    const vk::RenderPass& GetRenderPass() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    float GetExtentAspectRatio() const;
    vk::Result AcquireNextImage();
    vk::Result SubmitCommandBuffers(const vk::CommandBuffer& buffers, uint32_t& image_index);

private:
    void CreateSwapChain();
    void CreateRenderPass();
    void CreateDepthResources();
    void CreateFrameBuffers();
    void CreateSyncObjects();

private:
    static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats);
    static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes);
    vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
    vk::Format FindDepthFormat() const;

public:
    static constexpr int MAX_FRAMES_IN_FLIGHT_{2};

private:
    OdysseyEngine& engine_;
    vk::Extent2D window_extent_;
    vk::Format swap_chain_image_format_{};
    vk::Extent2D swap_chain_extent_{};
    vk::SwapchainKHR swap_chain_{};
    std::vector<vk::Image> swap_chain_images_{};
    std::vector<vk::ImageView> swap_chain_image_views_{};
    vk::RenderPass render_pass_{};
    std::vector<vk::Image> depth_images_{};
    std::vector<vk::DeviceMemory> depth_image_memories_{};
    std::vector<vk::ImageView> depth_image_views_{};
    std::vector<vk::Framebuffer> swap_chain_frame_buffers_{};
    std::vector<vk::Semaphore> image_available_semaphores_{};
    std::vector<vk::Semaphore> render_finished_semaphores_{};
    std::vector<vk::Fence> in_flight_fences_{};
    std::vector<vk::Fence> images_in_flight_{};
    size_t current_frame_{0};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_SWAP_CHAIN_H_