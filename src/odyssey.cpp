/**
 * @file odyssey.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey.h"

#include <array>
#include <stdexcept>

namespace odyssey {

Odyssey::Odyssey() : window_(std::make_shared<OdysseyWindow>(WIDTH, HEIGHT, "Odyssey")), engine_(std::make_shared<OdysseyEngine>(window_->GetSurfaceInfo(), window_->width(), window_->height())) {
}

void Odyssey::Run() {
    Draw();
}

void Odyssey::Draw() {
    try {
        auto image_index = engine_->AcquireNextImage();
        engine_->RecordCommandBuffer(image_index);
        engine_->SubmitCommandBuffers(image_index);
    } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
        engine_->RecreateSwapChain(window_->width(), window_->height());
    }
}

}  // namespace odyssey