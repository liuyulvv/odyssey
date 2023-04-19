/**
 * @file odyssey_window.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_window.h"

#include <QDebug>
#include <QGuiApplication>
#include <QWindow>
#include <stdexcept>

namespace odyssey {

#if defined(_WIN32)
vk::Win32SurfaceCreateInfoKHR OdysseyWindow::getSurfaceInfo() {
    auto wid = winId();
    auto* platformInterface = QGuiApplication::platformNativeInterface();
    auto* handle = platformInterface->nativeResourceForWindow("handle", this);
    auto* hwnd = static_cast<HWND>(handle);
    vk::Win32SurfaceCreateInfoKHR surfaceInfo{};
    surfaceInfo.setHwnd(hwnd);
    surfaceInfo.setHinstance(GetModuleHandle(nullptr));
    return surfaceInfo;
}
#endif

vk::Extent2D OdysseyWindow::getExtent() {
    return {static_cast<uint32_t>(width()), static_cast<uint32_t>(height())};
}

void OdysseyWindow::setMouseCallback(const std::function<void(OdysseyMouseEvent)>& callback) {
    mouseCallback = callback;
}

void OdysseyWindow::mouseDoubleClickEvent(QMouseEvent* event) {
    auto button = event->button();
    if (button == Qt::LeftButton) {
        auto screenX = event->position().x();
        auto screenY = event->position().y();
        auto windowWidth = width();
        auto windowHeight = height();
        float worldX = 1.0F;
        float worldY = 1.0F;
        screenToWorld(screenX, screenY, windowWidth, windowHeight, worldX, worldY);
        OdysseyMouseEvent mouseEvent{OdysseyMouseEventType::LEFT_DOUBLE, {static_cast<int>(screenX), static_cast<int>(screenY), worldX, worldY}};
        if (mouseCallback) {
            mouseCallback(mouseEvent);
        }
    }
}

void OdysseyWindow::mousePressEvent(QMouseEvent* event) {
    auto button = event->button();
    if (button == Qt::LeftButton || button == Qt::RightButton || button == Qt::MiddleButton) {
        auto screenX = event->position().x();
        auto screenY = event->position().y();
        auto windowWidth = width();
        auto windowHeight = height();
        float worldX = 1.0F;
        float worldY = 1.0F;
        screenToWorld(screenX, screenY, windowWidth, windowHeight, worldX, worldY);
        OdysseyMouseEventType type = button == Qt::LeftButton ? OdysseyMouseEventType::LEFT : button == Qt::RightButton ? OdysseyMouseEventType::RIGHT
                                                                                                                        : OdysseyMouseEventType::MID;
        OdysseyMouseEvent mouseEvent{type, {static_cast<int>(screenX), static_cast<int>(screenY), worldX, worldY}};
        if (mouseCallback) {
            mouseCallback(mouseEvent);
        }
    }
}

void OdysseyWindow::mouseMoveEvent(QMouseEvent* event) {
    auto button = event->button();
    if (button == Qt::LeftButton || button == Qt::RightButton || button == Qt::MiddleButton) {
        auto screenX = event->position().x();
        auto screenY = event->position().y();
        auto windowWidth = width();
        auto windowHeight = height();
        float worldX = 1.0F;
        float worldY = 1.0F;
        screenToWorld(screenX, screenY, windowWidth, windowHeight, worldX, worldY);
        OdysseyMouseEventType type = button == Qt::LeftButton ? OdysseyMouseEventType::LEFT_MOVE : button == Qt::RightButton ? OdysseyMouseEventType::RIGHT_MOVE
                                                                                                                             : OdysseyMouseEventType::MID_MOVE;
        OdysseyMouseEvent mouseEvent{type, {static_cast<int>(screenX), static_cast<int>(screenY), worldX, worldY}};
        if (mouseCallback) {
            mouseCallback(mouseEvent);
        }
    }
}

}  // namespace odyssey