/**
 * @file odyssey_device.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_device.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "odyssey_pipeline.h"
#include "odyssey_swap_chain.h"

namespace odyssey {

#if defined(_WIN32)
OdysseyDevice::OdysseyDevice(const vk::Win32SurfaceCreateInfoKHR& surfaceInfo, int width, int height) {
    createInstance();
    m_surface = m_instance.createWin32SurfaceKHR(surfaceInfo);
    setupDebugMessenger();
    pickPhysicalDevice();
    createLogicalDevice();
    createCommandPool();
}
#endif

OdysseyDevice::~OdysseyDevice() {
    m_device.waitIdle();
    m_device.destroyCommandPool(m_commandPool);
    m_device.destroy();
    if (m_enableValidationLayers) {
        m_instance.destroyDebugUtilsMessengerEXT(m_debugUtilsMessenger, nullptr, vk::DispatchLoaderDynamic(m_instance, reinterpret_cast<PFN_vkGetInstanceProcAddr>(m_instance.getProcAddr("vkGetInstanceProcAddr"))));
    }
    m_instance.destroySurfaceKHR(m_surface);
    m_instance.destroy();
}

const vk::Device& OdysseyDevice::device() const {
    return m_device;
}

const vk::SurfaceKHR& OdysseyDevice::surface() const {
    return m_surface;
}

SwapChainSupportDetails OdysseyDevice::getSwapChainSupport() const {
    return querySwapChainSupport(m_physical);
}

QueueFamilyIndices OdysseyDevice::findPhysicalQueueFamilies() const {
    return findQueueFamilies(m_physical);
}

vk::ImageView OdysseyDevice::createImageView(vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags) {
    vk::ImageViewCreateInfo viewInfo{};
    viewInfo
        .setImage(image)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(format);

    viewInfo.subresourceRange
        .setAspectMask(aspectFlags)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);
    return m_device.createImageView(viewInfo);
}

vk::Format OdysseyDevice::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const {
    for (const auto& format : candidates) {
        auto properties = m_physical.getFormatProperties(format);
        if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("No supported format found.");
}

void OdysseyDevice::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& memory) {
    vk::ImageCreateInfo imageInfo{};
    imageInfo
        .setImageType(vk::ImageType::e2D)
        .setMipLevels(1)
        .setArrayLayers(1)
        .setFormat(format)
        .setTiling(tiling)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(usage)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSamples(vk::SampleCountFlagBits::e1);
    imageInfo.extent
        .setWidth(width)
        .setHeight(height)
        .setDepth(1);
    image = m_device.createImage(imageInfo);
    auto memoryRequirements = m_device.getImageMemoryRequirements(image);
    vk::MemoryAllocateInfo allocateInfo{};
    allocateInfo
        .setAllocationSize(memoryRequirements.size)
        .setMemoryTypeIndex(findMemoryType(memoryRequirements.memoryTypeBits, properties));
    memory = m_device.allocateMemory(allocateInfo);
    m_device.bindImageMemory(image, memory, 0);
}

const vk::Queue& OdysseyDevice::getGraphicsQueue() const {
    return m_graphicsQueue;
}

const vk::Queue& OdysseyDevice::getPresentQueue() const {
    return m_presentQueue;
}

const vk::CommandPool& OdysseyDevice::getCommandPool() const {
    return m_commandPool;
}

void OdysseyDevice::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory) {
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo
        .setFlags(vk::BufferCreateFlags())
        .setSize(size)
        .setUsage(usage)
        .setSharingMode(vk::SharingMode::eExclusive);
    buffer = m_device.createBuffer(bufferInfo);
    auto memoryRequirements = m_device.getBufferMemoryRequirements(buffer);
    vk::MemoryAllocateInfo allocateInfo;
    allocateInfo
        .setAllocationSize(memoryRequirements.size)
        .setMemoryTypeIndex(findMemoryType(memoryRequirements.memoryTypeBits, properties));
    memory = m_device.allocateMemory(allocateInfo);
    m_device.bindBufferMemory(buffer, memory, 0);
}

void OdysseyDevice::createInstance() {
    if (m_enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available.");
    }
    checkExtensionsSupport();
    vk::ApplicationInfo appInfo{};
    appInfo
        .setPApplicationName("Odyssey")
        .setPEngineName("No Engine");

    auto extensions = getRequiredExtensions();

    vk::InstanceCreateInfo createInfo{};
    createInfo
        .setPApplicationInfo(&appInfo)
        .setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR)
        .setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
        .setPEnabledExtensionNames(extensions);

    if (m_enableValidationLayers) {
        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo
            .setEnabledLayerCount(static_cast<uint32_t>(m_validationLayers_.size()))
            .setPEnabledLayerNames(m_validationLayers_)
            .setPNext(&debugCreateInfo);
    } else {
        createInfo
            .setEnabledLayerCount(0)
            .setPNext(nullptr);
    }
    m_instance = vk::createInstance(createInfo);
}

void OdysseyDevice::setupDebugMessenger() {
    if (!m_enableValidationLayers) {
        return;
    }
    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    populateDebugMessengerCreateInfo(debugCreateInfo);
    m_debugUtilsMessenger = m_instance.createDebugUtilsMessengerEXT(debugCreateInfo, nullptr, vk::DispatchLoaderDynamic(m_instance, reinterpret_cast<PFN_vkGetInstanceProcAddr>(m_instance.getProcAddr("vkGetInstanceProcAddr"))));
}

void OdysseyDevice::pickPhysicalDevice() {
    auto devices = m_instance.enumeratePhysicalDevices();
    for (const auto& device : devices) {
        if (isPhysicalDeviceSuitable(device)) {
            m_physical = device;
            return;
        }
    }
    throw std::runtime_error("Failed to find a suitable GPU.");
}

void OdysseyDevice::createLogicalDevice() {
    auto indices = findQueueFamilies(m_physical);
    auto queuePriority = 1.0F;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    if (indices.graphicsFamily == indices.presentFamily) {
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo
            .setQueueCount(1)
            .setQueueFamilyIndex(indices.graphicsFamily)
            .setQueuePriorities(queuePriority);
        queueCreateInfos.push_back(queueCreateInfo);
    } else {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo
            .setQueueCount(1)
            .setQueueFamilyIndex(indices.graphicsFamily)
            .setQueuePriorities(queuePriority);
        queueCreateInfos.push_back(queueCreateInfo);
        queueCreateInfo.setQueueFamilyIndex(indices.presentFamily);
        queueCreateInfos.push_back(queueCreateInfo);
    }
    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.setSamplerAnisotropy(true);
    vk::DeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo
        .setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()))
        .setQueueCreateInfos(queueCreateInfos)
        .setEnabledExtensionCount(static_cast<uint32_t>(m_deviceExtensions.size()))
        .setPEnabledExtensionNames(m_deviceExtensions)
        .setPEnabledFeatures(&deviceFeatures);
    m_device = m_physical.createDevice(deviceCreateInfo);
    m_graphicsQueue = m_device.getQueue(indices.graphicsFamily, 0);
    m_presentQueue = m_device.getQueue(indices.presentFamily, 0);
}

void OdysseyDevice::createCommandPool() {
    auto queueFamilyIndices = findQueueFamilies(m_physical);
    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(queueFamilyIndices.graphicsFamily);
    m_commandPool = m_device.createCommandPool(poolInfo);
}

bool OdysseyDevice::checkValidationLayerSupport() {
    auto availableLayers = vk::enumerateInstanceLayerProperties();
    for (const auto& layerName : m_validationLayers_) {
        bool layerFound = false;
        for (const vk::LayerProperties& layerProperties : availableLayers) {
            if (std::string(layerProperties.layerName.data()) == layerName) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
            return false;
    }
    return true;
}

void OdysseyDevice::checkExtensionsSupport() {
    auto extensions = vk::enumerateInstanceExtensionProperties();
    std::unordered_set<std::string> available;
    for (const auto& extension : extensions) {
        available.insert(extension.extensionName);
    }
    auto requiredExtensions = getRequiredExtensions();
    for (const auto& required : requiredExtensions) {
        if (!available.contains(required)) {
            throw std::runtime_error("Missing required extension: " + std::string(required) + ".");
        }
    }
}

std::vector<const char*> OdysseyDevice::getRequiredExtensions() const {
#if defined(_WIN32)
    std::vector<const char*> extensions{"VK_KHR_surface", "VK_KHR_win32_surface"};
#endif
    if (m_enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    return extensions;
}

void OdysseyDevice::populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo
        .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
        .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
        .setPfnUserCallback(&debugCallback)
        .setPUserData(nullptr);
}

bool OdysseyDevice::isPhysicalDeviceSuitable(const vk::PhysicalDevice& device) {
    auto indices = findQueueFamilies(device);
    auto availableExtensions = device.enumerateDeviceExtensionProperties();
    std::unordered_set<std::string> requiredExtensions{m_deviceExtensions.begin(), m_deviceExtensions.end()};
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }
    bool extensionsSupported = requiredExtensions.empty();
    bool swapchainAdequate{false};
    if (extensionsSupported) {
        auto swapchainSupport = querySwapChainSupport(device);
        swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
    }
    auto supportedFeatures = device.getFeatures();
    return indices && extensionsSupported && swapchainAdequate && supportedFeatures.samplerAnisotropy;
}

QueueFamilyIndices OdysseyDevice::findQueueFamilies(const vk::PhysicalDevice& device) const {
    QueueFamilyIndices indices;
    auto properties = device.getQueueFamilyProperties();
    for (size_t i = 0; i < properties.size(); ++i) {
        const auto& property = properties[i];
        if (property.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = static_cast<uint32_t>(i);
            indices.hasGraphicsFamily = true;
        }
        if (device.getSurfaceSupportKHR(static_cast<uint32_t>(i), m_surface)) {
            indices.presentFamily = static_cast<uint32_t>(i);
            indices.hasPresentFamily = true;
        }
        if (indices) {
            break;
        }
    }
    return indices;
}

SwapChainSupportDetails OdysseyDevice::querySwapChainSupport(const vk::PhysicalDevice& device) const {
    SwapChainSupportDetails details{};
    details.capabilities = device.getSurfaceCapabilitiesKHR(m_surface);
    details.formats = device.getSurfaceFormatsKHR(m_surface);
    details.presentModes = device.getSurfacePresentModesKHR(m_surface);
    return details;
}

uint32_t OdysseyDevice::findMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memoryProperties{};
    m_physical.getMemoryProperties(&memoryProperties);
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if ((type_filter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return 0;
}

#if !defined(NODEBUG)

VKAPI_ATTR VkBool32 VKAPI_CALL OdysseyDevice::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, [[maybe_unused]] void* userData) {
    std::string message(callbackData->pMessage);
    std::string type;
    std::string severity;
    if (!message.empty()) {
        switch (messageType) {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
                type = "GENERAL";
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
                type = "VALIDATION";
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
                type = "PERFORMANCE";
                break;
            }
        }
        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                severity = "WARN";
                std::cerr << "[" << severity << "]"
                          << " Validation layer(" << type << "): " << message << std::endl;
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                severity = "ERROR";
                std::cerr << "[" << severity << "]"
                          << " Validation layer(" << type << "): " << message << std::endl;
                break;
            }
            default: {
                severity = "INFO";
                break;
            }
        }
    }
    return VK_FALSE;
}

#endif  // NODEBUG

}  // namespace odyssey