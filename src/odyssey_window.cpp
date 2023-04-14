/**
 * @file odyssey_window.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_window.h"

#include <QVulkanWindowRenderer>
#include <stdexcept>

#include "odyssey_window_render.h"

namespace odyssey {

OdysseyWindow::OdysseyWindow() : m_instance(new QVulkanInstance()) {
#if !defined(NODEBUG)
    m_instance->setLayers({"VK_LAYER_KHRONOS_validation"});
#endif
    if (!m_instance->create()) {
        throw std::runtime_error("Failed to create vulkan instance.");
    }
    setVulkanInstance(m_instance);
}

QVulkanWindowRenderer* OdysseyWindow::createRenderer() {
    return new OdysseyWindowRender(this);
}

}  // namespace odyssey