/**
 * @file odyssey_window.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_WINDOW_H_)
#define ODYSSEY_ODYSSEY_WINDOW_H_

#include <qpa/qplatformnativeinterface.h>

#include <QWindow>
#include <string>

#if defined(_WIN32)
#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vulkan.hpp"

namespace odyssey {

class OdysseyWindow : public QWindow {
public:
    OdysseyWindow(int width, int height, const std::string& window_name);
    ~OdysseyWindow() = default;

    OdysseyWindow() = delete;
    OdysseyWindow(const OdysseyWindow& odyssey_window) = delete;
    OdysseyWindow(OdysseyWindow&& odyssey_window) = delete;
    OdysseyWindow& operator=(const OdysseyWindow& odyssey_window) = delete;
    OdysseyWindow& operator=(OdysseyWindow&& odyssey_window) = delete;

public:
    vk::Extent2D GetExtent();

#if defined(_WIN32)
    vk::Win32SurfaceCreateInfoKHR GetSurfaceInfo();
#endif

private:
    vk::SurfaceKHR surface_{};
    std::string window_name_;
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_WINDOW_H_