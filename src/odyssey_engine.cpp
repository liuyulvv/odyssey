/**
 * @file odyssey_engine.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_engine.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "odyssey_model.h"
#include "odyssey_pipeline.h"
#include "odyssey_swap_chain.h"

namespace odyssey {

#if defined(_WIN32)
OdysseyEngine::OdysseyEngine(const vk::Win32SurfaceCreateInfoKHR& surfaceInfo, int width, int height) {
    createInstance();
    m_surface = m_instance.createWin32SurfaceKHR(surfaceInfo);
    setupDebugMessenger();
    pickPhysicalDevice();
    createLogicalDevice();
    createCommandPool();
    createPipelineLayout();
    recreateSwapChain(width, height);
    createCommandBuffers();

    std::vector<OdysseyModel::Vertex> vertices{
        {{0.0F, 0.0F}, {1.0F, 0.0F, 0.0F, 1.0F}},
        {{0.5F, 0.5F}, {0.0F, 1.0F, 0.0F, 1.0F}},
        {{0.5F, -0.5F}, {0.0F, 0.0F, 1.0F, 1.0F}},
        {{-0.5F, -0.5F}, {1.0F, 0.0F, 0.0F, 1.0F}},
        {{-0.5F, 0.5F}, {0.0F, 1.0F, 0.0F, 1.0F}},
        {{1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}},
    };
    m_model = std::make_unique<OdysseyModel>(this, vertices);
}
#endif

OdysseyEngine::~OdysseyEngine() {
    m_device.waitIdle();
    m_model.reset();
    m_swapChain.reset();
    m_pipelineLine.reset();
    m_device.freeCommandBuffers(m_commandPool, m_commandBuffers);
    m_device.destroyPipelineLayout(m_pipelineLayout);
    m_device.destroyCommandPool(m_commandPool);
    m_device.destroy();
    if (m_enableValidationLayers) {
        m_instance.destroyDebugUtilsMessengerEXT(m_debugUtilsMessenger, nullptr, vk::DispatchLoaderDynamic(m_instance, reinterpret_cast<PFN_vkGetInstanceProcAddr>(m_instance.getProcAddr("vkGetInstanceProcAddr"))));
    }
    m_instance.destroySurfaceKHR(m_surface);
    m_instance.destroy();
}

const vk::Device& OdysseyEngine::device() const {
    return m_device;
}

const vk::SurfaceKHR& OdysseyEngine::surface() const {
    return m_surface;
}

const vk::Instance& OdysseyEngine::instance() const {
    return m_instance;
}

const vk::PhysicalDevice& OdysseyEngine::physicalDevice() const {
    return m_physical;
}

SwapChainSupportDetails OdysseyEngine::getSwapChainSupport() const {
    return querySwapChainSupport(m_physical);
}

QueueFamilyIndices OdysseyEngine::findPhysicalQueueFamilies() const {
    return findQueueFamilies(m_physical);
}

vk::ImageView OdysseyEngine::createImageView(vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags) {
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

vk::Format OdysseyEngine::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const {
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

void OdysseyEngine::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& memory) {
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

const vk::Queue& OdysseyEngine::getGraphicsQueue() const {
    return m_graphicsQueue;
}

const vk::Queue& OdysseyEngine::getPresentQueue() const {
    return m_presentQueue;
}

const vk::CommandPool& OdysseyEngine::getCommandPool() const {
    return m_commandPool;
}

void OdysseyEngine::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory) {
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

vk::CommandBuffer OdysseyEngine::beginSingleTimeCommands() {
    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(m_commandPool)
        .setCommandBufferCount(1);
    auto commandBuffer = m_device.allocateCommandBuffers(allocateInfo);
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer.at(0).begin(beginInfo);
    return commandBuffer.at(0);
}

void OdysseyEngine::endSingleTimeCommands(vk::CommandBuffer& commandBuffer) {
    commandBuffer.end();
    vk::SubmitInfo submitInfo{};
    submitInfo
        .setCommandBufferCount(1)
        .setCommandBuffers(commandBuffer);
    m_graphicsQueue.submit(submitInfo);
    m_graphicsQueue.waitIdle();
    m_device.freeCommandBuffers(m_commandPool, commandBuffer);
}

uint32_t OdysseyEngine::acquireNextImage() {
    return m_swapChain->acquireNextImage();
}

void OdysseyEngine::recordCommandBuffer(uint32_t imageIndex) {
    vk::CommandBufferBeginInfo beginInfo{};
    m_commandBuffers[imageIndex].begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo
        .setRenderPass(m_swapChain->getRenderPass())
        .setFramebuffer(m_swapChain->getFrameBuffer(imageIndex));
    renderPassInfo.renderArea
        .setOffset({0, 0})
        .setExtent(m_swapChain->getSwapChainExtent());
    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].setColor({0.17F, 0.17F, 0.17F, 1.0F});
    clearValues[1].setDepthStencil({1.0F, 0});
    renderPassInfo
        .setClearValueCount(static_cast<uint32_t>(clearValues.size()))
        .setClearValues(clearValues);

    m_commandBuffers[imageIndex].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport{};
    viewport
        .setX(0.0F)
        .setY(0.0F)
        .setWidth(static_cast<float>(m_swapChain->getSwapChainExtent().width))
        .setHeight(static_cast<float>(m_swapChain->getSwapChainExtent().height))
        .setMinDepth(0.0F)
        .setMaxDepth(1.0F);
    vk::Rect2D scissor{{0, 0}, m_swapChain->getSwapChainExtent()};

    m_commandBuffers[imageIndex].setViewport(0, viewport);
    m_commandBuffers[imageIndex].setScissor(0, scissor);

    m_pipelineLine->bind(m_commandBuffers[imageIndex]);
    m_model->bind(m_commandBuffers[imageIndex]);
    m_model->draw(m_commandBuffers[imageIndex]);

    m_commandBuffers[imageIndex].endRenderPass();
    m_commandBuffers[imageIndex].end();
}

void OdysseyEngine::submitCommandBuffers(uint32_t imageIndex) {
    m_swapChain->submitCommandBuffers(m_commandBuffers[imageIndex], imageIndex);
}

void OdysseyEngine::createInstance() {
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

void OdysseyEngine::setupDebugMessenger(){
    if (!m_enableValidationLayers) {
        return;
    }
    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    populateDebugMessengerCreateInfo(debugCreateInfo);
    m_debugUtilsMessenger = m_instance.createDebugUtilsMessengerEXT(debugCreateInfo, nullptr, vk::DispatchLoaderDynamic(m_instance, reinterpret_cast<PFN_vkGetInstanceProcAddr>(m_instance.getProcAddr("vkGetInstanceProcAddr"))));
}

void OdysseyEngine::pickPhysicalDevice() {
    auto devices = m_instance.enumeratePhysicalDevices();
    for (const auto& device : devices) {
        if (isPhysicalDeviceSuitable(device)) {
            m_physical = device;
            return;
        }
    }
    throw std::runtime_error("Failed to find a suitable GPU.");
}

void OdysseyEngine::createLogicalDevice() {
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

void OdysseyEngine::createCommandPool() {
    auto queueFamilyIndices = findQueueFamilies(m_physical);
    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(queueFamilyIndices.graphicsFamily);
    m_commandPool = m_device.createCommandPool(poolInfo);
}

void OdysseyEngine::createPipelineLayout() {
    vk::PipelineLayoutCreateInfo pipelineInfo{};
    pipelineInfo
        .setSetLayoutCount(0)
        .setPSetLayouts(nullptr)
        .setPushConstantRangeCount(0)
        .setPPushConstantRanges(nullptr);
    m_pipelineLayout = m_device.createPipelineLayout(pipelineInfo);
    if (!m_pipelineLayout) {
        throw std::runtime_error("Failed to create pipeline layout.");
    }
}

void OdysseyEngine::recreateSwapChain(int width, int height) {
    m_device.waitIdle();
    m_swapChain.reset(nullptr);
    m_swapChain = std::make_unique<OdysseySwapChain>(this, width, height);
    m_pipelineLine = createPipeline("shaders/shader.vert.spv", "shaders/shader.frag.spv", vk::PrimitiveTopology::eTriangleList, 1.0F);
}

std::unique_ptr<OdysseyPipeline> OdysseyEngine::createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, vk::PrimitiveTopology primitiveTopology, float lineWidth) {
    auto pipelineConfig = OdysseyPipeline::defaultPipelineConfigInfo(primitiveTopology, lineWidth);
    pipelineConfig.renderPass = m_swapChain->getRenderPass();
    pipelineConfig.pipelineLayout = m_pipelineLayout;
    return std::make_unique<OdysseyPipeline>(this, vertShaderPath, fragShaderPath, pipelineConfig);
}

void OdysseyEngine::createCommandBuffers() {
    m_commandBuffers.resize(m_swapChain->getImageCount());
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(m_commandPool)
        .setCommandBufferCount(static_cast<uint32_t>(m_commandBuffers.size()));
    m_commandBuffers = m_device.allocateCommandBuffers(allocInfo);
}

bool OdysseyEngine::checkValidationLayerSupport() {
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

void OdysseyEngine::checkExtensionsSupport() {
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

std::vector<const char*> OdysseyEngine::getRequiredExtensions() const {
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

void OdysseyEngine::populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo) {
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

bool OdysseyEngine::isPhysicalDeviceSuitable(const vk::PhysicalDevice& device) {
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

QueueFamilyIndices OdysseyEngine::findQueueFamilies(const vk::PhysicalDevice& device) const {
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

SwapChainSupportDetails OdysseyEngine::querySwapChainSupport(const vk::PhysicalDevice& device) const {
    SwapChainSupportDetails details{};
    details.capabilities = device.getSurfaceCapabilitiesKHR(m_surface);
    details.formats = device.getSurfaceFormatsKHR(m_surface);
    details.presentModes = device.getSurfacePresentModesKHR(m_surface);
    return details;
}

uint32_t OdysseyEngine::findMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags properties) {
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

VKAPI_ATTR VkBool32 VKAPI_CALL OdysseyEngine::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, [[maybe_unused]] void* userData) {
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