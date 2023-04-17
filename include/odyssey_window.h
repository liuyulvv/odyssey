/**
 * @file odyssey_window.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_WINDOW_H_)
#define ODYSSEY_ODYSSEY_WINDOW_H_

#include <QWidget>
#include <QWindow>
#include <string>

#include "vulkan/vulkan.hpp"

namespace odyssey {

class OdysseyWindow : public QWindow {
public:
    OdysseyWindow(int width, int height, const std::string& window_name);
    ~OdysseyWindow();

    OdysseyWindow() = delete;
    OdysseyWindow(const OdysseyWindow& odyssey_window) = delete;
    OdysseyWindow(OdysseyWindow&& odyssey_window) = delete;
    OdysseyWindow& operator=(const OdysseyWindow& odyssey_window) = delete;
    OdysseyWindow& operator=(OdysseyWindow&& odyssey_window) = delete;

// public:
//     QPaintEngine* paintEngine() const;
//     void paintEvent(QPaintEvent* event) override;
//     void resizeEvent(QResizeEvent* event) override;

public:
    bool ShouldClose() const;
    static void PollEvents();
    static vk::Extent2D GetExtent();
    vk::SurfaceKHR GetSurface() const;

private:
    vk::SurfaceKHR surface_{};
    static int width_;
    static int height_;
    std::string window_name_;
    static double mouse_x_;
    static double mouse_y_;
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_WINDOW_H_