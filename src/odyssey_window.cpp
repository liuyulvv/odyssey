/**
 * @file odyssey_window.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_window.h"

#include <qpa/qplatformnativeinterface.h>

#include <QByteArray>
#include <QGuiApplication>
#include <stdexcept>

namespace odyssey {

int OdysseyWindow::width_{0};
int OdysseyWindow::height_{0};
double OdysseyWindow::mouse_x_{0.0};
double OdysseyWindow::mouse_y_{0.0};

OdysseyWindow::OdysseyWindow(int width, int height, const std::string& window_name) : window_name_(window_name) {
    setSurfaceType(QSurface::VulkanSurface);
    show();
    auto* interface = QGuiApplication::platformNativeInterface();
    // auto wid = winId();
    // auto window = windowHandle();
    auto* p = interface->nativeResourceForWindow(QByteArrayLiteral("vkSurface"), this);
    VkSurfaceKHR c_surface = p ? (*static_cast<VkSurfaceKHR*>(p)) : VK_NULL_HANDLE;
    surface_ = vk::SurfaceKHR(c_surface);
    // QWidget::setAttribute(Qt::WA_PaintOnScreen);
    // setFocusPolicy(Qt::WheelFocus);
    // setMouseTracking(true);
    width_ = width;
    height_ = height;
}

OdysseyWindow::~OdysseyWindow() {
}

// QPaintEngine* OdysseyWindow::paintEngine() const {
//     return nullptr;
// }

// void OdysseyWindow::paintEvent(QPaintEvent* event) {
// }

// void OdysseyWindow::resizeEvent(QResizeEvent* event) {
// }

bool OdysseyWindow::ShouldClose() const {
    return false;
}

void OdysseyWindow::PollEvents() {
}

vk::Extent2D OdysseyWindow::GetExtent() {
    return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)};
}

vk::SurfaceKHR OdysseyWindow::GetSurface() const {
    return surface_;
}

}  // namespace odyssey