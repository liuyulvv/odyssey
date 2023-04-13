/**
 * @file odyssey.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey.h"

#include <array>
#include <stdexcept>

namespace odyssey {

Odyssey::Odyssey() : window_(std::make_shared<OdysseyWindow>(WIDTH, HEIGHT, "Odyssey")), engine_(std::make_shared<OdysseyEngine>(window_)) {
    LoadModel();
    CreatePipelineLayout();
    RecreateSwapChain();
    CreateCommandBuffers();
    gui_ = std::make_unique<OdysseyGUI>(engine_, window_, swap_chain_->GetRenderPass());
}

Odyssey::~Odyssey() {
    gui_.release();
    engine_->Device().destroyPipelineLayout(pipeline_layout_);
}

void Odyssey::Run() {
    MainLoop();
}

void Odyssey::MainLoop() {
    while (!window_->ShouldClose()) {
        OdysseyWindow::PollEvents();
        Draw();
    }
    engine_->Device().waitIdle();
}

void Odyssey::CreatePipelineLayout() {
    vk::PipelineLayoutCreateInfo pipeline_info;
    pipeline_info
        .setSetLayoutCount(0)
        .setPSetLayouts(nullptr)
        .setPushConstantRangeCount(0)
        .setPPushConstantRanges(nullptr);
    pipeline_layout_ = engine_->Device().createPipelineLayout(pipeline_info);
    if (!pipeline_layout_) {
        throw std::runtime_error("Failed to create pipeline layout.");
    }
}

std::unique_ptr<OdysseyPipeline> Odyssey::CreatePipeline(const std::string& vert_shader_path, const std::string& frag_shader_path, vk::PrimitiveTopology primitive_topology, float line_width) {
    auto pipeline_config = OdysseyPipeline::DefaultPipelineConfigInfo(primitive_topology, line_width);
    pipeline_config.render_pass_ = swap_chain_->GetRenderPass();
    pipeline_config.pipeline_layout_ = pipeline_layout_;
    return std::make_unique<OdysseyPipeline>(engine_, vert_shader_path, frag_shader_path, pipeline_config);
}

void Odyssey::CreateCommandBuffers() {
    command_buffers_.resize(swap_chain_->GetImageCount());
    vk::CommandBufferAllocateInfo alloc_info{};
    alloc_info
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(engine_->GetCommandPool())
        .setCommandBufferCount(static_cast<uint32_t>(command_buffers_.size()));
    command_buffers_ = engine_->Device().allocateCommandBuffers(alloc_info);
}

void Odyssey::Draw() {
    try {
        auto image_index = swap_chain_->AcquireNextImage();
        RecordCommandBuffer(image_index);
        swap_chain_->SubmitCommandBuffers(command_buffers_[image_index], image_index);
    } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
        RecreateSwapChain();
    }
}

void Odyssey::LoadModel() {
    std::vector<OdysseyModel::Vertex> vertices{
        {{0.0F, 0.0F}, {1.0F, 0.0F, 0.0F, 1.0F}},
        {{0.5F, 0.5F}, {0.0F, 1.0F, 0.0F, 1.0F}},
        {{0.5F, -0.5F}, {0.0F, 0.0F, 1.0F, 1.0F}},
        {{-0.5F, -0.5F}, {1.0F, 0.0F, 0.0F, 1.0F}},
        {{-0.5F, 0.5F}, {0.0F, 1.0F, 0.0F, 1.0F}},
        {{1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}},
    };
    model_ = std::make_unique<OdysseyModel>(engine_, vertices);
}

void Odyssey::RecreateSwapChain() {
    auto extent = OdysseyWindow::GetExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = OdysseyWindow::GetExtent();
        glfwWaitEvents();
    }
    engine_->Device().waitIdle();
    swap_chain_.reset(nullptr);
    swap_chain_ = std::make_unique<OdysseySwapChain>(engine_, extent);
    pipeline_line_ = CreatePipeline("shaders/shader.vert.spv", "shaders/shader.frag.spv", vk::PrimitiveTopology::eTriangleList, 1.0F);
}

void Odyssey::RecordCommandBuffer(uint32_t image_index) {
    vk::CommandBufferBeginInfo begin_info{};
    command_buffers_[image_index].begin(begin_info);

    vk::RenderPassBeginInfo render_pass_info{};
    render_pass_info
        .setRenderPass(swap_chain_->GetRenderPass())
        .setFramebuffer(swap_chain_->GetFrameBuffer(image_index));
    render_pass_info.renderArea
        .setOffset({0, 0})
        .setExtent(swap_chain_->GetSwapChainExtent());
    std::array<vk::ClearValue, 2> clear_values{};
    clear_values[0].setColor({0.17F, 0.17F, 0.17F, 1.0F});
    clear_values[1].setDepthStencil({1.0F, 0});
    render_pass_info
        .setClearValueCount(static_cast<uint32_t>(clear_values.size()))
        .setClearValues(clear_values);

    command_buffers_[image_index].beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

    vk::Viewport viewport{};
    viewport
        .setX(0.0F)
        .setY(0.0F)
        .setWidth(static_cast<float>(swap_chain_->GetSwapChainExtent().width))
        .setHeight(static_cast<float>(swap_chain_->GetSwapChainExtent().height))
        .setMinDepth(0.0F)
        .setMaxDepth(1.0F);
    vk::Rect2D scissor{{0, 0}, swap_chain_->GetSwapChainExtent()};

    command_buffers_[image_index].setViewport(0, viewport);
    command_buffers_[image_index].setScissor(0, scissor);

    gui_->Draw(command_buffers_[image_index]);

    pipeline_line_->Bind(command_buffers_[image_index]);
    model_->Bind(command_buffers_[image_index]);
    model_->Draw(command_buffers_[image_index]);

    command_buffers_[image_index].endRenderPass();
    command_buffers_[image_index].end();
}

}  // namespace odyssey