/**
 * @file odyssey_engine.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_ENGINE_H_)
#define ODYSSEY_ODYSSEY_ENGINE_H_

#include <memory>
#include <vector>

#include "odyssey_model.h"
#include "odyssey_window.h"

#if defined(_WIN32)
#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vulkan.hpp"

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

class OdysseyEngine {
public:
#if defined(_WIN32)
    explicit OdysseyEngine(const vk::Win32SurfaceCreateInfoKHR& surfaceInfo, int width, int height);
#endif
    ~OdysseyEngine();
    OdysseyEngine(const OdysseyEngine& odysseyEngine) = delete;
    OdysseyEngine(OdysseyEngine&& odysseyEngine) = delete;
    OdysseyEngine& operator=(const OdysseyEngine& odysseyEngine) = delete;
    OdysseyEngine& operator=(OdysseyEngine&& odysseyEngine) = delete;

public:
    const vk::Device& device() const;
    const vk::SurfaceKHR& surface() const;
    const vk::Instance& instance() const;
    const vk::PhysicalDevice& physicalDevice() const;
    SwapChainSupportDetails getSwapChainSupport() const;
    QueueFamilyIndices findPhysicalQueueFamilies() const;
    vk::ImageView createImageView(vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags);
    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
    void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& memory);
    const vk::Queue& getGraphicsQueue() const;
    const vk::Queue& getPresentQueue() const;
    const vk::CommandPool& getCommandPool() const;
    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory);
    vk::CommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(vk::CommandBuffer& commandBuffer);
    void recreateSwapChain(int width, int height);
    uint32_t acquireNextImage();
    void recordCommandBuffer(uint32_t imageIndex);
    void submitCommandBuffers(uint32_t imageIndex);
    void loadModel(const std::vector<OdysseyModel::Vertex>& vertices);
    void clearModel();

private:
    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();
    void createPipelineLayout();
    std::unique_ptr<OdysseyPipeline> createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, vk::PrimitiveTopology primitiveTopology, float lineWidth);
    void createCommandBuffers();

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
    vk::PipelineLayout m_pipelineLayout{};
    std::unique_ptr<OdysseySwapChain> m_swapChain{};
    std::unique_ptr<OdysseyPipeline> m_pipelineLine{};
    std::vector<vk::CommandBuffer> m_commandBuffers{};
    std::vector<std::unique_ptr<OdysseyModel>> m_models{};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_ENGINE_H_