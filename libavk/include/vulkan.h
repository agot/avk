#ifndef AVK_VK_APP_H_
#define AVK_VK_APP_H_

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_layer.h>

#if defined(_WIN64) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#include <vulkan/vulkan_win32.h>
#elif defined(__linux__)
#define VK_USE_PLATFORM_WAYLAND_KHR
#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <vulkan/vulkan_wayland.h>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace avk {

class Vulkan {
public:
    Vulkan(GLFWwindow* window, const std::string& appName);
    virtual ~Vulkan();

private:
    struct Surface {
        vk::SurfaceKHR surface;
        vk::Extent2D extent;
    };
    struct Swapchain {
        vk::SwapchainKHR swapchain;
        std::vector<vk::Image> images;
        std::vector<vk::ImageView> imageViews;
    };
    struct DepthBuffer {
        vk::Image image;
        vk::DeviceMemory memory;
        vk::ImageView imageView;
    };
    struct CommandBuffer {
        vk::CommandPool commandPool;
        vk::CommandBuffer commandBuffer;
    };
    /*
    struct Layout {
        vk::DescriptorSetLayout descriptorSetLayout;
        vk::PipelineLayout pipelineLayout;
    };
    struct Descriptor {
        vk::DescriptorPool descriptorPool;
        vk::DescriptorSet descriptorSet;
    };
    */

    vk::Instance _instance;
    vk::Device _device;
    std::shared_ptr<Surface> _surface;
    std::shared_ptr<Swapchain> _swapchain;
    std::shared_ptr<DepthBuffer> _depthBuffer;
    vk::RenderPass _renderPass;
    std::vector<vk::Framebuffer> _framebuffers;
    std::shared_ptr<CommandBuffer> _commandBuffer;
    //std::shared_ptr<Layout> _layout;

    static vk::Instance createInstance(const vk::ApplicationInfo& applicationInfo, const std::vector<const char*>& layers, const std::vector<const char*>& extensions);
    static vk::PhysicalDevice getPhysicalDevice(const vk::Instance& instance);
    static uint32_t getGraphicsQueueFamilyIndex(const vk::PhysicalDevice& physicalDevice);
    static vk::Device createDevice(const vk::PhysicalDevice& physicalDevice, uint32_t graphicsQueueFamilyIndex);
    static std::shared_ptr<Vulkan::Surface> createSurface(GLFWwindow* window, const vk::Instance& instance);
    static vk::SurfaceFormatKHR getSurfaceFormat(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const vk::Format& format);
    static uint32_t getPresentQueueFamilyIndex(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, uint32_t graphicsQueueFamilyIndex);
    static std::shared_ptr<Swapchain> createSwapchain(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const std::shared_ptr<Surface>& surface, const vk::Format& colorFormat, uint32_t graphicsQueueFamilyIndex, uint32_t presentQueueFamilyIndex);
    static std::shared_ptr<DepthBuffer> createDepthBuffer(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, uint32_t width, uint32_t height);
    static vk::RenderPass createRenderPass(const vk::Device& device, const vk::Format& colorFormat, const vk::Format& depthFormat);
    static std::vector<vk::Framebuffer> createFrameBuffers(const vk::Device& device, const std::shared_ptr<DepthBuffer>& depthBuffer, const std::shared_ptr<Swapchain>& swapchain, const vk::RenderPass& renderPass, uint32_t width, uint32_t height);
    static std::shared_ptr<CommandBuffer> createCommandBuffer(const vk::Device& device, uint32_t graphicsQueueFamilyIndex);
    //static std::shared_ptr<Layout> createLayout(const vk::Device& device);
    //static std::shared_ptr<Descriptor> createDescriptor(const vk::Device& device, const vk::DescriptorSetLayout& descriptorSetLayout);
};

} // namespace avk

#endif
