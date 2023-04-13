/**
 * @file odyssey_gui.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-12
 */

#include "odyssey_gui.h"

#include <array>

#include "imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace odyssey {

OdysseyGUI::OdysseyGUI(std::shared_ptr<OdysseyEngine> engine, std::shared_ptr<OdysseyWindow> window, const vk::RenderPass &render_pass) : engine_(std::move(engine)), window_(std::move(window)), render_pass_(render_pass) {
    std::array<vk::DescriptorPoolSize, 11> pool_sizes;
    pool_sizes.at(0).setType(vk::DescriptorType::eSampler).setDescriptorCount(1000);
    pool_sizes.at(1).setType(vk::DescriptorType::eCombinedImageSampler).setDescriptorCount(1000);
    pool_sizes.at(2).setType(vk::DescriptorType::eSampledImage).setDescriptorCount(1000);
    pool_sizes.at(3).setType(vk::DescriptorType::eStorageImage).setDescriptorCount(1000);
    pool_sizes.at(4).setType(vk::DescriptorType::eUniformTexelBuffer).setDescriptorCount(1000);
    pool_sizes.at(5).setType(vk::DescriptorType::eStorageTexelBuffer).setDescriptorCount(1000);
    pool_sizes.at(6).setType(vk::DescriptorType::eUniformBuffer).setDescriptorCount(1000);
    pool_sizes.at(7).setType(vk::DescriptorType::eStorageBuffer).setDescriptorCount(1000);
    pool_sizes.at(8).setType(vk::DescriptorType::eUniformBufferDynamic).setDescriptorCount(1000);
    pool_sizes.at(9).setType(vk::DescriptorType::eStorageBufferDynamic).setDescriptorCount(1000);
    pool_sizes.at(10).setType(vk::DescriptorType::eInputAttachment).setDescriptorCount(1000);
    vk::DescriptorPoolCreateInfo pool_info;
    pool_info
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
        .setPoolSizeCount(static_cast<uint32_t>(pool_sizes.size()))
        .setPoolSizes(pool_sizes)
        .setMaxSets(1000);
    pool_ = engine_->Device().createDescriptorPool(pool_info);
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForVulkan(window_->GetWindow(), true);
    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = engine_->Instance();
    init_info.PhysicalDevice = engine_->PhysicalDevice();
    init_info.Device = engine_->Device();
    init_info.Queue = engine_->GetGraphicsQueue();
    init_info.DescriptorPool = pool_;
    init_info.MinImageCount = 2;
    init_info.ImageCount = 2;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    ImGui_ImplVulkan_Init(&init_info, render_pass_);
    auto command_buffer = engine_->BeginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
    engine_->EndSingleTimeCommands(command_buffer);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

OdysseyGUI::~OdysseyGUI() {
    engine_->Device().destroyDescriptorPool(pool_);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void OdysseyGUI::Draw(vk::CommandBuffer &command_buffer) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    DrawTopMenuBar();
    ImGui::Render();
    auto *gui_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(gui_data, command_buffer);
}

void OdysseyGUI::DrawTopMenuBar() {
    if (top_menu_bar_) {
        auto *viewport = reinterpret_cast<ImGuiViewportP *>(ImGui::GetMainViewport());
        auto window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        if (ImGui::BeginViewportSideBar("TopMenuBar", viewport, ImGuiDir_Up, ImGui::GetFrameHeight(), window_flags | ImGuiWindowFlags_NoBackground)) {
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    }
                    if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    }
                    if (ImGui::MenuItem("Import", "Ctrl+I")) {
                    }
                    if (ImGui::MenuItem("Output", "Ctrl+Shift+S")) {
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::End();
        }
    }
}

}  // namespace odyssey