#pragma once

/**
 * @file odyssey_window.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <qpa/qplatformnativeinterface.h>

#include <QMouseEvent>
#include <QWindow>
#include <functional>

#include "odyssey_header.h"
#include "odyssey_mouse_event.h"

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
    void setMouseCallback(const std::function<void(const OdysseyMouseEvent&)>& callback);

private:
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

private:
    static void screenToWorld(int screenX, int screenY, int width, int height, float& worldX, float& worldY);

private:
    std::function<void(const OdysseyMouseEvent&)> mouseCallback{};
};

}  // namespace odyssey