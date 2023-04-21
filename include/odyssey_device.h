#pragma once

/**
 * @file odyssey_device.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <memory>
#include <vector>

#include "odyssey_header.h"
#include "odyssey_model.h"
#include "odyssey_object.h"
#include "odyssey_window.h"

namespace odyssey {

class OdysseyPipeline;
class OdysseySwapChain;

struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;

    bool hasGraphicsFamily = false;
    bool hasPresentFamily = false;

    operator bool() {
        return hasGraphicsFamily && hasPresentFamily;
    }
};

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class OdysseyDevice {
public:
#if defined(_WIN32)
    explicit OdysseyDevice(const vk::Win32SurfaceCreateInfoKHR& surfaceInfo, int width, int height);
#endif
    ~OdysseyDevice();
    OdysseyDevice(const OdysseyDevice& odysseyDevice) = delete;
    OdysseyDevice(OdysseyDevice&& odysseyDevice) = delete;
    OdysseyDevice& operator=(const OdysseyDevice& odysseyDevice) = delete;
    OdysseyDevice& operator=(OdysseyDevice&& odysseyDevice) = delete;

public:
    const vk::Device& device() const;
    const vk::SurfaceKHR& surface() const;
    SwapChainSupportDetails getSwapChainSupport() const;
    QueueFamilyIndices findPhysicalQueueFamilies() const;
    vk::ImageView createImageView(vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags);
    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
    void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& memory);
    const vk::Queue& getGraphicsQueue() const;
    const vk::Queue& getPresentQueue() const;
    const vk::CommandPool& getCommandPool() const;
    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory);

private:
    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();

private:
    bool checkValidationLayerSupport();
    void checkExtensionsSupport();
    std::vector<const char*> getRequiredExtensions() const;
    static void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);
    bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& device);
    QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) const;
    SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device) const;
    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

private:
#if defined(NODEBUG)
    bool m_enableValidationLayers = false;
#else
    bool m_enableValidationLayers = true;
    std::vector<const char*> m_validationLayers_{"VK_LAYER_KHRONOS_validation"};
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                        void* userData);
    vk::DebugUtilsMessengerEXT m_debugUtilsMessenger;
#endif  // NODEBUG

private:
    vk::Instance m_instance{};
    vk::SurfaceKHR m_surface{};
    vk::PhysicalDevice m_physical{};

private:
#if defined(_WIN32)
    std::vector<const char*> m_deviceExtensions = {"VK_KHR_swapchain"};
#else
    std::vector<const char*> m_deviceExtensions = {"VK_KHR_portability_subset", "VK_KHR_swapchain"};
#endif

private:
    vk::Device m_device{};
    vk::Queue m_graphicsQueue{};
    vk::Queue m_presentQueue{};
    vk::CommandPool m_commandPool{};
};

}  // namespace odyssey