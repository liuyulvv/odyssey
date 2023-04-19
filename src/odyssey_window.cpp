/**
 * @file odyssey_window.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_window.h"

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

}  // namespace odyssey