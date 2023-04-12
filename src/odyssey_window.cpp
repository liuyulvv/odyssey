/**
 * @file odyssey_window.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_window.h"

#include <stdexcept>

namespace odyssey {

OdysseyWindow::OdysseyWindow(int width, int height, const std::string& window_name) : width_(width), height_(height), window_name_(window_name) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window_ = glfwCreateWindow(width_, height_, window_name.c_str(), nullptr, nullptr);
    glfwSetMouseButtonCallback(window_, MouseButtonCallback);
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

vk::Extent2D OdysseyWindow::GetExtent() const {
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

}  // namespace odyssey