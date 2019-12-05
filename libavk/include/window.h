#ifndef AVK_WINDOW_H_
#define AVK_WINDOW_H_

#include <chrono>

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "vulkan.h"

namespace avk {

template<typename T>
class Window {
public:
    Window() {
        if ( glfwInit() != GLFW_TRUE ) {
            throw std::runtime_error("Failed to glfwInit.");
        }
        if ( glfwVulkanSupported() == 0 ) {
            throw std::runtime_error("Vulkan not supported.");
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindow = glfwCreateWindow(_width, _height, "Vulkan window", nullptr, nullptr);
        if ( glfwWindow == nullptr ) {
            throw std::runtime_error("Failed to create GLFW window.");
        }
        _t = std::make_unique<T>(glfwWindow, "test");
    }

    virtual ~Window() {
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
    }

    void update(const std::chrono::microseconds& deltaTime) {
        if ( glfwWindowShouldClose(glfwWindow) == 0 ) {
            glfwPollEvents();
        }
    }

private:
    GLFWwindow* glfwWindow;
    std::unique_ptr<T> _t;
    int32_t _width = 800;
    int32_t _height = 600;

};

}

#endif
