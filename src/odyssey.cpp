/**
 * @file odyssey.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey.h"

#include <array>
#include <stdexcept>

namespace odyssey {

Odyssey::Odyssey() {
    CreatePipelineLayout();
    CreatePipeline();
    CreateCommandBuffers();
}

Odyssey::~Odyssey() {
    engine_.Device().destroyPipelineLayout(pipeline_layout_);
}

void Odyssey::Run() {
    MainLoop();
}

void Odyssey::MainLoop() {
    while (!window_.ShouldClose()) {
        OdysseyWindow::PollEvents();
        Draw();
    }
    engine_.Device().waitIdle();
}

void Odyssey::CreatePipelineLayout() {
    vk::PipelineLayoutCreateInfo pipeline_info;
    pipeline_info
        .setSetLayoutCount(0)
        .setPSetLayouts(nullptr)
        .setPushConstantRangeCount(0)
        .setPPushConstantRanges(nullptr);
    pipeline_layout_ = engine_.Device().createPipelineLayout(pipeline_info);
    if (!pipeline_layout_) {
        throw std::runtime_error("Failed to create pipeline layout.");
    }
}

void Odyssey::CreatePipeline() {
    auto pipeline_config = OdysseyPipeline::DefaultPipelineConfigInfo(swap_chain_.GetWidth(), swap_chain_.GetHeight());
    pipeline_config.render_pass_ = swap_chain_.GetRenderPass();
    pipeline_config.pipeline_layout_ = pipeline_layout_;
    pipeline_ = std::make_unique<OdysseyPipeline>(engine_, "shaders/vert_1.vert.spv", "shaders/frag_1.frag.spv", pipeline_config);
}

void Odyssey::CreateCommandBuffers() {
    command_buffers_.resize(swap_chain_.GetImageCount());
    vk::CommandBufferAllocateInfo alloc_info{};
    alloc_info
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(engine_.GetCommandPool())
        .setCommandBufferCount(static_cast<uint32_t>(command_buffers_.size()));
    command_buffers_ = engine_.Device().allocateCommandBuffers(alloc_info);
    for (size_t i = 0; i < command_buffers_.size(); ++i) {
        vk::CommandBufferBeginInfo begin_info{};
        command_buffers_[i].begin(begin_info);

        vk::RenderPassBeginInfo render_pass_info{};
        render_pass_info
            .setRenderPass(swap_chain_.GetRenderPass())
            .setFramebuffer(swap_chain_.GetFrameBuffer(i));
        render_pass_info.renderArea
            .setOffset({0, 0})
            .setExtent(swap_chain_.GetSwapChainExtent());

        std::array<vk::ClearValue, 2> clear_values{};
        clear_values[0].setColor({0.17F, 0.17F, 0.17F, 1.0F});
        clear_values[1].setDepthStencil({1.0F, 0});
        render_pass_info
            .setClearValueCount(static_cast<uint32_t>(clear_values.size()))
            .setClearValues(clear_values);

        command_buffers_[i].beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
        pipeline_->Bind(command_buffers_[i]);

        command_buffers_[i].draw(3, 1, 0, 0);

        command_buffers_[i].endRenderPass();
        command_buffers_[i].end();
    }
}

void Odyssey::Draw() {
    uint32_t image_index{};
    auto res = swap_chain_.AcquireNextImage(image_index);
    if (res != vk::Result::eSuccess && res != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire swap chain image.");
    }
    res = swap_chain_.SubmitCommandBuffers(command_buffers_[image_index], image_index);
    if (res != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swap chain image.");
    }
}

}  // namespace odyssey