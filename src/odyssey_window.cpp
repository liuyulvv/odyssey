/**
 * @file odyssey_window.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_window.h"

#include <stdexcept>

namespace odyssey {

int OdysseyWindow::width_{0};
int OdysseyWindow::height_{0};
double OdysseyWindow::mouse_x_{0.0};
double OdysseyWindow::mouse_y_{0.0};

OdysseyWindow::OdysseyWindow(int width, int height, const std::string& window_name) : window_name_(window_name) {
    width_ = width;
    height_ = height;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window_ = glfwCreateWindow(width_, height_, window_name.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window_, this);
    glfwSetMouseButtonCallback(window_, MouseButtonCallback);
    glfwSetCursorPosCallback(window_, CursorPositionCallback);
    glfwSetFramebufferSizeCallback(window_, ResizedCallback);
}

OdysseyWindow::~OdysseyWindow() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}

bool OdysseyWindow::ShouldClose() const {
    return glfwWindowShouldClose(window_);
}

void OdysseyWindow::PollEvents() {
    glfwPollEvents();
}

void OdysseyWindow::CreateWindowSurface(vk::Instance& instance, vk::SurfaceKHR& surface) {
    VkSurfaceKHR c_surface = nullptr;
    if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), window_, nullptr, &c_surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
    surface = vk::SurfaceKHR(c_surface);
}

vk::Extent2D OdysseyWindow::GetExtent() {
    return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)};
}

GLFWwindow* OdysseyWindow::GetWindow() const {
    return window_;
}

void OdysseyWindow::KeyBoardCallback([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scan_code, int action, [[maybe_unused]] int mods) {
}

void OdysseyWindow::MouseButtonCallback([[maybe_unused]] GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
    }
}

void OdysseyWindow::CursorPositionCallback([[maybe_unused]] GLFWwindow* window, double x, double y) {
    mouse_x_ = x;
    mouse_y_ = y;
}

void OdysseyWindow::ResizedCallback(GLFWwindow* window, int width, int height) {
    auto* odyssey_window = reinterpret_cast<OdysseyWindow*>(window);
    OdysseyWindow::width_ = width;
    OdysseyWindow::height_ = height;
}

}  // namespace odyssey