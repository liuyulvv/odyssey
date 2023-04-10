/**
 * @file odyssey_engine.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_engine.h"

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "fmt/format.h"

namespace odyssey {

OdysseyEngine::OdysseyEngine(OdysseyWindow& window) : window_(window) {
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateCommandPool();
}

OdysseyEngine::~OdysseyEngine() {
    device_.destroyCommandPool(command_pool_);
    device_.destroy();
    if (enable_validation_layers_) {
        instance_.destroyDebugUtilsMessengerEXT(debug_utils_messenger_, nullptr, vk::DispatchLoaderDynamic(instance_, reinterpret_cast<PFN_vkGetInstanceProcAddr>(instance_.getProcAddr("vkGetInstanceProcAddr"))));
    }
    instance_.destroySurfaceKHR(surface_);
    instance_.destroy();
}

const vk::Device& OdysseyEngine::Device() const {
    return device_;
}

const vk::SurfaceKHR& OdysseyEngine::Surface() const {
    return surface_;
}

SwapChainSupportDetails OdysseyEngine::GetSwapChainSupport() const {
    return QuerySwapChainSupport(physical_);
}

QueueFamilyIndices OdysseyEngine::FindPhysicalQueueFamilies() const {
    return FindQueueFamilies(physical_);
}

vk::ImageView OdysseyEngine::CreateImageView(vk::Image& image, vk::Format format, vk::ImageAspectFlags aspect_flags) {
    vk::ImageViewCreateInfo view_info;
    view_info
        .setImage(image)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(format);

    view_info.subresourceRange
        .setAspectMask(aspect_flags)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);
    return device_.createImageView(view_info);
}

vk::Format OdysseyEngine::FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const {
    for (const auto& format : candidates) {
        auto properties = physical_.getFormatProperties(format);
        if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("No supported format found.");
}

void OdysseyEngine::CreateImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& memory) {
    vk::ImageCreateInfo image_info;
    image_info
        .setImageType(vk::ImageType::e2D)
        .setMipLevels(1)
        .setArrayLayers(1)
        .setFormat(format)
        .setTiling(tiling)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(usage)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSamples(vk::SampleCountFlagBits::e1);
    image_info.extent
        .setWidth(width)
        .setHeight(height)
        .setDepth(1);
    image = device_.createImage(image_info);
    if (!image) {
        throw std::runtime_error("Failed to create image.");
    }
    auto memory_requirements = device_.getImageMemoryRequirements(image);
    vk::MemoryAllocateInfo allocate_info;
    allocate_info
        .setAllocationSize(memory_requirements.size)
        .setMemoryTypeIndex(FindMemoryType(memory_requirements.memoryTypeBits, properties));
    memory = device_.allocateMemory(allocate_info);
    if (!memory) {
        throw std::runtime_error("Failed to allocate image memory.");
    }
    device_.bindImageMemory(image, memory, 0);
}

const vk::Queue& OdysseyEngine::GetGraphicsQueue() const {
    return graphics_queue_;
}

const vk::Queue& OdysseyEngine::GetPresentQueue() const {
    return present_queue_;
}

void OdysseyEngine::CreateInstance() {
    if (enable_validation_layers_ && !CheckValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available.");
    }
    CheckExtensionsSupport();
    vk::ApplicationInfo app_info{};
    app_info
        .setPApplicationName("Odyssey")
        .setPEngineName("No Engine");

    auto extensions = GetRequiredExtensions();

    vk::InstanceCreateInfo create_info{};
    create_info
        .setPApplicationInfo(&app_info)
        .setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR)
        .setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
        .setPEnabledExtensionNames(extensions);

    if (enable_validation_layers_) {
        vk::DebugUtilsMessengerCreateInfoEXT debug_create_info;
        PopulateDebugMessengerCreateInfo(debug_create_info);
        create_info
            .setEnabledLayerCount(static_cast<uint32_t>(validation_layers_.size()))
            .setPEnabledLayerNames(validation_layers_)
            .setPNext(&debug_create_info);
    } else {
        create_info
            .setEnabledLayerCount(0)
            .setPNext(nullptr);
    }
    instance_ = vk::createInstance(create_info);
}

void OdysseyEngine::SetupDebugMessenger() {
    if (!enable_validation_layers_) {
        return;
    }
    vk::DebugUtilsMessengerCreateInfoEXT debug_create_info;
    PopulateDebugMessengerCreateInfo(debug_create_info);
    debug_utils_messenger_ = instance_.createDebugUtilsMessengerEXT(debug_create_info, nullptr, vk::DispatchLoaderDynamic(instance_, reinterpret_cast<PFN_vkGetInstanceProcAddr>(instance_.getProcAddr("vkGetInstanceProcAddr"))));
}

void OdysseyEngine::CreateSurface() {
    window_.CreateWindowSurface(instance_, surface_);
}

void OdysseyEngine::PickPhysicalDevice() {
    auto devices = instance_.enumeratePhysicalDevices();
    for (const auto& device : devices) {
        if (IsPhysicalDeviceSuitable(device)) {
            physical_ = device;
            break;
        }
    }
    if (!physical_) {
        throw std::runtime_error("Failed to find a suitable GPU.");
    }
}

void OdysseyEngine::CreateLogicalDevice() {
    auto indices = FindQueueFamilies(physical_);
    auto queue_priority = 1.0F;
    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
    if (indices.graphics_family_ == indices.present_family_) {
        vk::DeviceQueueCreateInfo queue_create_info;
        queue_create_info
            .setQueueCount(1)
            .setQueueFamilyIndex(indices.graphics_family_)
            .setQueuePriorities(queue_priority);
        queue_create_infos.push_back(queue_create_info);
    } else {
        vk::DeviceQueueCreateInfo queue_create_info;
        queue_create_info
            .setQueueCount(1)
            .setQueueFamilyIndex(indices.graphics_family_)
            .setQueuePriorities(queue_priority);
        queue_create_infos.push_back(queue_create_info);
        queue_create_info.setQueueFamilyIndex(indices.present_family_);
        queue_create_infos.push_back(queue_create_info);
    }
    vk::PhysicalDeviceFeatures device_features;
    device_features.setSamplerAnisotropy(true);
    vk::DeviceCreateInfo device_create_info;
    device_create_info
        .setQueueCreateInfoCount(static_cast<uint32_t>(queue_create_infos.size()))
        .setQueueCreateInfos(queue_create_infos)
        .setEnabledExtensionCount(static_cast<uint32_t>(device_extensions_.size()))
        .setPEnabledExtensionNames(device_extensions_)
        .setPEnabledFeatures(&device_features);
    device_ = physical_.createDevice(device_create_info);
    graphics_queue_ = device_.getQueue(indices.graphics_family_, 0);
    present_queue_ = device_.getQueue(indices.present_family_, 0);
}

void OdysseyEngine::CreateCommandPool() {
    auto queue_family_indices = FindQueueFamilies(physical_);
    vk::CommandPoolCreateInfo pool_info;
    pool_info
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(queue_family_indices.graphics_family_);
    command_pool_ = device_.createCommandPool(pool_info);
}

bool OdysseyEngine::CheckValidationLayerSupport() {
    auto available_layers = vk::enumerateInstanceLayerProperties();
    for (const auto& layer_name : validation_layers_) {
        bool layer_found = false;
        for (const vk::LayerProperties& layer_properties : available_layers) {
            if (std::string(layer_properties.layerName.data()) == layer_name) {
                layer_found = true;
                break;
            }
        }
        if (!layer_found) {
            return false;
        }
    }
    return true;
}

void OdysseyEngine::CheckExtensionsSupport() {
    auto extensions = vk::enumerateInstanceExtensionProperties();
    std::unordered_set<std::string> available;
    for (const auto& extension : extensions) {
        available.insert(extension.extensionName);
    }
    auto required_extensions = GetRequiredExtensions();
    for (const auto& required : required_extensions) {
        if (!available.contains(required)) {
            throw std::runtime_error(fmt::format("Missing required extension: {}.", required));
        }
    }
}

std::vector<const char*> OdysseyEngine::GetRequiredExtensions() const {
    uint32_t glfw_extension_count = 0;
    const auto** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
    if (enable_validation_layers_) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    return extensions;
}

void OdysseyEngine::PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info) {
    create_info
        .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
        .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
        .setPfnUserCallback(&DebugCallback)
        .setPUserData(nullptr);
}

bool OdysseyEngine::IsPhysicalDeviceSuitable(const vk::PhysicalDevice& device) {
    auto indices = FindQueueFamilies(device);
    auto available_extensions = device.enumerateDeviceExtensionProperties();
    std::unordered_set<std::string> required_extensions{device_extensions_.begin(), device_extensions_.end()};
    for (const auto& extension : available_extensions) {
        required_extensions.erase(extension.extensionName);
    }
    bool extensions_supported = required_extensions.empty();
    bool swap_chain_adequate{false};
    if (extensions_supported) {
        auto swap_chain_support = QuerySwapChainSupport(device);
        swap_chain_adequate = !swap_chain_support.formats_.empty() && !swap_chain_support.present_modes_.empty();
    }
    auto supported_features = device.getFeatures();
    return indices && extensions_supported && swap_chain_adequate && supported_features.samplerAnisotropy;
}

QueueFamilyIndices OdysseyEngine::FindQueueFamilies(const vk::PhysicalDevice& device) const {
    QueueFamilyIndices indices;
    auto properties = device.getQueueFamilyProperties();
    for (size_t i = 0; i < properties.size(); ++i) {
        const auto& property = properties[i];
        if (property.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphics_family_ = static_cast<uint32_t>(i);
            indices.has_graphics_family_ = true;
        }
        if (device.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface_)) {
            indices.present_family_ = static_cast<uint32_t>(i);
            indices.has_present_family_ = true;
        }
        if (indices) {
            break;
        }
    }
    return indices;
}

SwapChainSupportDetails OdysseyEngine::QuerySwapChainSupport(const vk::PhysicalDevice& device) const {
    SwapChainSupportDetails details{};
    details.capabilities_ = device.getSurfaceCapabilitiesKHR(surface_);
    details.formats_ = device.getSurfaceFormatsKHR(surface_);
    details.present_modes_ = device.getSurfacePresentModesKHR(surface_);
    return details;
}

uint32_t OdysseyEngine::FindMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memory_properties;
    physical_.getMemoryProperties(&memory_properties);
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((type_filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return 0;
}

#if !defined(NODEBUG)

VKAPI_ATTR VkBool32 VKAPI_CALL OdysseyEngine::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, [[maybe_unused]] void* user_data) {
    std::string message(callback_data->pMessage);
    std::string type;
    std::string severity;
    if (!message.empty()) {
        switch (message_type) {
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
        switch (message_severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                severity = "WARN";
                std::cerr << fmt::format("[{}] Validation layer({}): {}", severity, type, message) << std::endl;
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                severity = "ERROR";
                std::cerr << fmt::format("[{}] Validation layer({}): {}", severity, type, message) << std::endl;
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