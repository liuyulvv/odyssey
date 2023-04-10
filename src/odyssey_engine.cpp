/**
 * @file odyssey_engine.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_engine.h"

#include <string>
#include <unordered_set>
#include <vector>

#include "fmt/format.h"

namespace odyssey {

OdysseyEngine::OdysseyEngine(OdysseyWindow& window) : window_(window) {
    CreateInstance();
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
        .setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
        .setPEnabledExtensionNames(extensions);
}

bool OdysseyEngine::CheckValidationLayerSupport() {
    uint32_t layer_count{0};
    auto res = vk::enumerateInstanceLayerProperties(&layer_count, nullptr);
    assert(res == vk::Result::eSuccess);
    std::vector<vk::LayerProperties> available_layers(layer_count);
    res = vk::enumerateInstanceLayerProperties(&layer_count, available_layers.data());
    assert(res == vk::Result::eSuccess);
    for (const auto& layer_name : validation_layers) {
        bool layer_found = false;
        for (const vk::LayerProperties& layer_properties : available_layers) {
            if (std::string(layer_properties.layerName) == layer_name) {
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
    uint32_t extension_count{0};
    auto res = vk::enumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    assert(res == vk::Result::eSuccess);
    std::vector<vk::ExtensionProperties> extensions(extension_count);
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

}  // namespace odyssey