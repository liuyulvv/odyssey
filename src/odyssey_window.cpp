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
vk::Win32SurfaceCreateInfoKHR OdysseyWindow::GetSurfaceInfo() {
    auto wid = winId();
    auto* platform_interface = QGuiApplication::platformNativeInterface();
    auto* handle = platform_interface->nativeResourceForWindow("handle", this);
    auto* hwnd = static_cast<HWND>(handle);
    vk::Win32SurfaceCreateInfoKHR surface_info{};
    surface_info.setHwnd(hwnd);
    surface_info.setHinstance(GetModuleHandle(nullptr));
    return surface_info;
}
#endif

vk::Extent2D OdysseyWindow::GetExtent() {
    return {static_cast<uint32_t>(width()), static_cast<uint32_t>(height())};
}

}  // namespace odyssey