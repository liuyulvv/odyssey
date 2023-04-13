/**
 * @file odyssey_window.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_WINDOW_H_)
#define ODYSSEY_ODYSSEY_WINDOW_H_

#if !defined(GLFW_INCLUDE_VULKAN)
#define GLFW_INCLUDE_VULKAN
#endif  // GLFW_INCLUDE_VULKAN

#include <string>

#include "GLFW/glfw3.h"
#include "vulkan/vulkan.hpp"

namespace odyssey {

class OdysseyWindow {
public:
    OdysseyWindow(int width, int height, const std::string& window_name);
    ~OdysseyWindow();

    OdysseyWindow() = delete;
    OdysseyWindow(const OdysseyWindow& odyssey_window) = delete;
    OdysseyWindow(OdysseyWindow&& odyssey_window) = delete;
    OdysseyWindow& operator=(const OdysseyWindow& odyssey_window) = delete;
    OdysseyWindow& operator=(OdysseyWindow&& odyssey_window) = delete;

public:
    bool ShouldClose() const;
    static void PollEvents();
    void CreateWindowSurface(vk::Instance& instance, vk::SurfaceKHR& surface);
    static vk::Extent2D GetExtent();
    GLFWwindow* GetWindow() const;

private:
    static void KeyBoardCallback(GLFWwindow* window, int key, int scan_code, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void CursorPositionCallback(GLFWwindow* window, double x, double y);
    static void ResizedCallback(GLFWwindow* window, int width, int height);

private:
    static int width_;
    static int height_;
    std::string window_name_;
    GLFWwindow* window_{nullptr};
    static double mouse_x_;
    static double mouse_y_;
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_WINDOW_H_