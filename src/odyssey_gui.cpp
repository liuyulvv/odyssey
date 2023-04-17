/**
 * @file odyssey_gui.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-12
 */

#include "odyssey_gui.h"

#include <array>

namespace odyssey {

OdysseyGUI::OdysseyGUI(std::shared_ptr<OdysseyEngine> engine, std::shared_ptr<OdysseyWindow> window, const vk::RenderPass &render_pass) : engine_(std::move(engine)), window_(std::move(window)), render_pass_(render_pass) {
    
}

OdysseyGUI::~OdysseyGUI() {

}

void OdysseyGUI::Draw(vk::CommandBuffer &command_buffer) {

}

void OdysseyGUI::DrawTopMenuBar() {
    
}

}  // namespace odyssey