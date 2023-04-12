/**
 * @file odyssey_gui.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-12
 */

#if !defined(ODYSSEY_ODYSSEY_GUI_H_)
#define ODYSSEY_ODYSSEY_GUI_H_

#include "odyssey_engine.h"
#include "odyssey_swap_chain.h"
#include "odyssey_window.h"
#include "vulkan/vulkan.hpp"

namespace odyssey {

class OdysseyGUI {
public:
    OdysseyGUI(OdysseyEngine& engine, OdysseyWindow& window, OdysseySwapChain& swap_chain);
    ~OdysseyGUI();
    OdysseyGUI() = delete;
    OdysseyGUI(const OdysseyGUI& odyssey_gui) = delete;
    OdysseyGUI(OdysseyGUI&& odyssey_gui) = delete;
    OdysseyGUI& operator=(const OdysseyGUI& odyssey_gui) = delete;
    OdysseyGUI& operator=(OdysseyGUI&& odyssey_gui) = delete;

public:
    void Draw(vk::CommandBuffer& command_buffer);

private:
    void Draw();

private:
    OdysseyEngine& engine_;
    OdysseyWindow& window_;
    OdysseySwapChain& swap_chain_;
    vk::DescriptorPool pool_;

    bool top_menu_bar_{true};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_GUI_H_