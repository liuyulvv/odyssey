/**
 * @file odyssey_engine.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_ENGINE_H_)
#define ODYSSEY_ODYSSEY_ENGINE_H_

#include <vector>

#include "odyssey_window.h"
#include "vulkan/vulkan.hpp"

namespace odyssey {

struct QueueFamilyIndices {
    uint32_t graphics_family_;
    uint32_t present_family_;

    bool has_graphics_family_ = false;
    bool has_present_family_ = false;

    operator bool() {
        return has_graphics_family_ && has_present_family_;
    }
};

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities_;
    std::vector<vk::SurfaceFormatKHR> formats_;
    std::vector<vk::PresentModeKHR> present_modes_;
};

class OdysseyEngine {
public:
    explicit OdysseyEngine(OdysseyWindow& window);
    ~OdysseyEngine();

    OdysseyEngine() = delete;
    OdysseyEngine(const OdysseyEngine& odyssey_engine) = delete;
    OdysseyEngine(OdysseyEngine&& odyssey_engine) = delete;
    OdysseyEngine& operator=(const OdysseyEngine& odyssey_engine) = delete;
    OdysseyEngine& operator=(OdysseyEngine&& odyssey_engine) = delete;

public:
    const vk::Device& Device() const;

private:
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommandPool();

private:
    bool CheckValidationLayerSupport();
    void CheckExtensionsSupport();
    std::vector<const char*> GetRequiredExtensions() const;
    static void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info);
    bool IsPhysicalDeviceSuitable(const vk::PhysicalDevice& device);
    QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device);
    SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& device);

private:
    OdysseyWindow& window_;

#if defined(NODEBUG)
    bool enable_validation_layers_ = false;
#else
    bool enable_validation_layers_ = true;
    std::vector<const char*> validation_layers_{"VK_LAYER_KHRONOS_validation"};
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                        VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                        void* user_data);
    vk::DebugUtilsMessengerEXT debug_utils_messenger_;
#endif  // NODEBUG

    vk::Instance instance_{};
    vk::SurfaceKHR surface_{};
    vk::PhysicalDevice physical_{};
#if defined(_WIN32)
    std::vector<const char*> device_extensions_ = {"VK_KHR_swapchain"};
#else
    std::vector<const char*> device_extensions_ = {"VK_KHR_portability_subset", "VK_KHR_swapchain"};
#endif
    vk::Device device_{};
    vk::Queue graphics_queue_{};
    vk::Queue present_queue_{};
    vk::CommandPool command_pool_{};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_ENGINE_H_