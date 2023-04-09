/**
 * @file odyssey_window.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_WINDOW_H_)
#define ODYSSEY_ODYSSEY_WINDOW_H_

#include <string>

#include "GLFW/glfw3.h"

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
    void PollEvents();

private:
    int width_;
    int height_;
    std::string window_name_;
    GLFWwindow* window_{nullptr};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_WINDOW_H_