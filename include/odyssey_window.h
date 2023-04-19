/**
 * @file odyssey_window.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_WINDOW_H_)
#define ODYSSEY_ODYSSEY_WINDOW_H_

#include <qpa/qplatformnativeinterface.h>

#include <QMouseEvent>
#include <QWindow>
#include <functional>

#include "odyssey_mouse_event.h"

#if defined(_WIN32)
#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vulkan.hpp"

namespace odyssey {

class OdysseyWindow : public QWindow {
public:
    OdysseyWindow() = default;
    ~OdysseyWindow() = default;

    OdysseyWindow(const OdysseyWindow& odysseyWindow) = delete;
    OdysseyWindow(OdysseyWindow&& odysseyWindow) = delete;
    OdysseyWindow& operator=(const OdysseyWindow& odysseyWindow) = delete;
    OdysseyWindow& operator=(OdysseyWindow&& odysseyWindow) = delete;

public:
    vk::Extent2D getExtent();
#if defined(_WIN32)
    vk::Win32SurfaceCreateInfoKHR getSurfaceInfo();
#endif

public:
    void setMouseCallback(const std::function<void(OdysseyMouseEvent)>& callback);

private:
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private:
    vk::SurfaceKHR m_surface{};
    std::function<void(OdysseyMouseEvent)> mouseCallback{};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_WINDOW_H_