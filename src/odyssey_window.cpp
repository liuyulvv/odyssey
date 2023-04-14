/**
 * @file odyssey_window.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_window.h"

#include <QVulkanWindowRenderer>
#include <stdexcept>

#include "odyssey_render.h"

namespace odyssey {

OdysseyWindow::OdysseyWindow() {
}

QVulkanWindowRenderer* OdysseyWindow::createRenderer() {
    return new OdysseyRender(this);
}

}  // namespace odyssey