#include "vulkan.h"

#include <iostream>
#include <limits>

#include <gsl/gsl>

namespace avk {

Vulkan::Vulkan(GLFWwindow* window, const std::string& appName) {
    /* init instance */
    vk::ApplicationInfo applicationInfo(
        appName.c_str(),
        1,
        "avk",
        1,
        VK_API_VERSION_1_2
    );
    std::vector<const char*> layers = {
#ifndef NDEBUG
        "VK_LAYER_KHRONOS_validation"
#endif
    };
    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(_WIN32) || defined(_WIN64)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(__linux__)
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#endif
#ifndef NDEBUG
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#endif
    };

    int width = 0;
    int height = 0;
    glfwGetWindowSize(window, &width, &height);

    _instance = createInstance(applicationInfo, layers, extensions);
    auto physicalDevice = getPhysicalDevice(_instance);
    auto graphicsQueueFamilyIndex = getGraphicsQueueFamilyIndex(physicalDevice);
    _device = createDevice(physicalDevice, graphicsQueueFamilyIndex);
    _surface = createSurface(window, _instance);
    auto surfaceFormat = getSurfaceFormat(physicalDevice, _surface->surface, vk::Format::eB8G8R8A8Unorm);
    auto presentQueueFamilyIndex = getPresentQueueFamilyIndex(physicalDevice, _surface->surface, graphicsQueueFamilyIndex);
    _swapchain = createSwapchain(physicalDevice, _device, _surface, surfaceFormat.format, graphicsQueueFamilyIndex, presentQueueFamilyIndex);
    _depthBuffer = createDepthBuffer(physicalDevice, _device, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    _renderPass = createRenderPass(_device, surfaceFormat.format, vk::Format::eD16Unorm);
    _framebuffers = createFrameBuffers(_device, _depthBuffer, _swapchain, _renderPass, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    _commandBuffer = createCommandBuffer(_device, graphicsQueueFamilyIndex);
    //_layout = createLayout(_device);

    std::array<vk::ClearValue, 2> clearValues;
    clearValues[0].color = vk::ClearColorValue(
        std::array<float, 4>({1.0F, 0.0F, 0.0F, 0.0F})
    );
    clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0F, 0);

    vk::Semaphore imageAcquiredSemaphore = _device.createSemaphore(vk::SemaphoreCreateInfo());
    constexpr uint64_t fenceTimeout = 100000000;
    vk::ResultValue<uint32_t> currentBuffer = _device.acquireNextImageKHR(
        _swapchain->swapchain,
        fenceTimeout,
        imageAcquiredSemaphore,
        nullptr
    );
    if ( currentBuffer.result != vk::Result::eSuccess ) {
        throw std::runtime_error("Faild to get currentBuffer.");
    }
    if ( currentBuffer.value >= _framebuffers.size() ) {
        throw std::runtime_error("Invalid currentBuffer Value");
    }

    vk::RenderPassBeginInfo renderPassBeginInfo(
        _renderPass,
        _framebuffers[currentBuffer.value],
        vk::Rect2D(vk::Offset2D(0, 0), _surface->extent),
        clearValues
    );

    _commandBuffer->commandBuffer.begin(vk::CommandBufferBeginInfo());
    _commandBuffer->commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    _commandBuffer->commandBuffer.endRenderPass();
    _commandBuffer->commandBuffer.end();

    vk::Fence drawFence = _device.createFence(vk::FenceCreateInfo());

    vk::Queue graphicsQueue = _device.getQueue(graphicsQueueFamilyIndex, 0);
    vk::Queue presentQueue = _device.getQueue(presentQueueFamilyIndex, 0 );

    vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo submitInfo(imageAcquiredSemaphore, waitDestinationStageMask, _commandBuffer->commandBuffer);
    graphicsQueue.submit(submitInfo, drawFence);

    while ( vk::Result::eTimeout == _device.waitForFences(drawFence, VK_TRUE, fenceTimeout) ) {
    }

    vk::Result result = presentQueue.presentKHR(
        vk::PresentInfoKHR(
            {},
            _swapchain->swapchain,
            currentBuffer.value
        )
    );
    switch ( result ) {
    case vk::Result::eSuccess:
        break;
    case vk::Result::eSuboptimalKHR:
        std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
        break;
    default:
        throw std::runtime_error("Present unexpected result.");
    }
    _device.destroyFence(drawFence);
    _device.destroySemaphore(imageAcquiredSemaphore);
}

Vulkan::~Vulkan() {
    //_layout = nullptr;
    _commandBuffer = nullptr;
    for ( auto& framebuffer : _framebuffers ) {
        _device.destroyFramebuffer(framebuffer);
    }
    _device.destroyRenderPass(_renderPass);
    _depthBuffer = nullptr;
    _swapchain = nullptr;
    _surface = nullptr;
    _device.destroy();
    _instance.destroy();
}

vk::Instance Vulkan::createInstance(const vk::ApplicationInfo& applicationInfo, const std::vector<const char*>& layers, const std::vector<const char*>& extensions) {
    vk::InstanceCreateInfo instanceCreateInfo(
        {},
        &applicationInfo,
        layers.size(),
        layers.data(),
        extensions.size(),
        extensions.data()
    );
    return vk::createInstance(instanceCreateInfo);
}

vk::PhysicalDevice Vulkan::getPhysicalDevice(const vk::Instance& instance) {
    std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
    return physicalDevices.front();
}

uint32_t Vulkan::getGraphicsQueueFamilyIndex(const vk::PhysicalDevice& physicalDevice) {
    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    auto itr = std::find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(), [](const auto& property) {
        return property.queueFlags & vk::QueueFlagBits::eGraphics;
    });
    auto graphicsQueueFamilyIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), itr));
    if ( graphicsQueueFamilyIndex >= queueFamilyProperties.size() ) {
        throw std::runtime_error("vk::QueueFlagBits::eGraphics not found.");
    }
    return graphicsQueueFamilyIndex;
}

vk::Device Vulkan::createDevice(const vk::PhysicalDevice& physicalDevice, uint32_t graphicsQueueFamilyIndex) {
    float queuePriority = 0.0F;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
        vk::DeviceQueueCreateFlags(),
        graphicsQueueFamilyIndex,
        1,
        &queuePriority
    );
    std::vector<const char*> layers = {
    };
    std::vector<const char*> extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    vk::DeviceCreateInfo deviceCreateInfo(
        vk::DeviceCreateFlags(),
        deviceQueueCreateInfo,
        layers,
        extensions
    );
    return physicalDevice.createDevice(deviceCreateInfo);
}

std::shared_ptr<Vulkan::Surface> Vulkan::createSurface(GLFWwindow* window, const vk::Instance& instance) {
    VkSurfaceKHR surface = 0;
    glfwCreateWindowSurface(instance, window, nullptr, &surface);
    int width = 0;
    int height = 0;
    glfwGetWindowSize(window, &width, &height);
    return std::shared_ptr<Surface>(new Surface{vk::SurfaceKHR(surface), vk::Extent2D(width, height)}, [&instance](gsl::owner<Surface*> ptr) {
        instance.destroySurfaceKHR(ptr->surface);
        delete ptr;
    });
}

vk::SurfaceFormatKHR Vulkan::getSurfaceFormat(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const vk::Format& format) {
    auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
    auto itr = std::find_if(surfaceFormats.begin(), surfaceFormats.end(), [&format](const auto& surfaceFormat) {
        return format == surfaceFormat.format;
    });
    if ( itr == surfaceFormats.end() ) {
        throw std::runtime_error("Surface format not found");
    }
    return *itr;
}

uint32_t Vulkan::getPresentQueueFamilyIndex(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, uint32_t graphicsQueueFamilyIndex) {
    /* TODO: what is this.
      auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    */
    auto isSupport = physicalDevice.getSurfaceSupportKHR(graphicsQueueFamilyIndex, surface);
    if ( isSupport == VK_FALSE ) {
        throw std::runtime_error("Failed to get presentQueuefamilyindex.");
        // TODO: fallback
    }
    uint32_t presentQueueFamilyIndex = graphicsQueueFamilyIndex;
    return presentQueueFamilyIndex;
}

std::shared_ptr<Vulkan::Swapchain> Vulkan::createSwapchain(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const std::shared_ptr<Surface>& surface, const vk::Format& colorFormat, uint32_t graphicsQueueFamilyIndex, uint32_t presentQueueFamilyIndex) {
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface->surface);
    vk::Extent2D swapchainExtent;
    if ( surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max() ) {
        swapchainExtent = surface->extent;
    }
    else {
        swapchainExtent = surfaceCapabilities.currentExtent;
    }
    vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;
    vk::SurfaceTransformFlagBitsKHR preTransform =
        ( surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity )
        ? vk::SurfaceTransformFlagBitsKHR::eIdentity
        : surfaceCapabilities.currentTransform;
    vk::CompositeAlphaFlagBitsKHR compositeAlpha =
        ( surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied )
        ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
        : ( surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied )
        ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
        : ( surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit )
        ? vk::CompositeAlphaFlagBitsKHR::eInherit
        : vk::CompositeAlphaFlagBitsKHR::eOpaque;

    vk::SwapchainCreateInfoKHR swapchainCreateInfo(
        vk::SwapchainCreateFlagsKHR(),
        surface->surface,
        surfaceCapabilities.minImageCount,
        colorFormat,
        vk::ColorSpaceKHR::eSrgbNonlinear,
        swapchainExtent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        vk::SharingMode::eExclusive,
        {},
        preTransform,
        compositeAlpha,
        swapchainPresentMode,
        VK_TRUE,
        nullptr
    );

    if ( graphicsQueueFamilyIndex != presentQueueFamilyIndex ) {
        std::array<uint32_t, 2> queueFamilyIndices = {
            graphicsQueueFamilyIndex,
            presentQueueFamilyIndex
        };
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    auto swapchain = device.createSwapchainKHR(swapchainCreateInfo);
    std::vector<vk::Image> images = device.getSwapchainImagesKHR(swapchain);
    std::vector<vk::ImageView> imageViews;
    imageViews.reserve(images.size());
    vk::ComponentMapping componentMapping(
        vk::ComponentSwizzle::eR,
        vk::ComponentSwizzle::eG,
        vk::ComponentSwizzle::eB,
        vk::ComponentSwizzle::eA
    );
    vk::ImageSubresourceRange subResourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    for ( auto image : images ) {
        vk::ImageViewCreateInfo imageViewCreateInfo(
            vk::ImageViewCreateFlags(),
            image,
            vk::ImageViewType::e2D,
            colorFormat,
            componentMapping,
            subResourceRange
        );
        imageViews.push_back(device.createImageView(imageViewCreateInfo));
    }
    return std::shared_ptr<Swapchain>(new Swapchain{swapchain, images, imageViews}, [&device](gsl::owner<Swapchain*> ptr) {
        for ( auto& imageView : ptr->imageViews ) {
            device.destroyImageView(imageView);
        }
        device.destroySwapchainKHR(ptr->swapchain);
        delete ptr;
    });
}

std::shared_ptr<Vulkan::DepthBuffer> Vulkan::createDepthBuffer(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, uint32_t width, uint32_t height) {
    const vk::Format depthFormat = vk::Format::eD16Unorm;
    vk::FormatProperties formatProperties = physicalDevice.getFormatProperties(depthFormat);

    vk::ImageTiling tiling;
    if ( formatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment ) {
        tiling = vk::ImageTiling::eLinear;
    }
    else if ( formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment ) {
        tiling = vk::ImageTiling::eOptimal;
    }
    else {
        throw std::runtime_error( "DepthStencilAttachment is not supported for D16Unorm depth format." );
    }
    vk::ImageCreateInfo imageCreateInfo(
        vk::ImageCreateFlags(),
        vk::ImageType::e2D,
        depthFormat,
        vk::Extent3D(vk::Extent2D(width, height), 1),
        1,
        1,
        vk::SampleCountFlagBits::e1,
        tiling,
        vk::ImageUsageFlagBits::eDepthStencilAttachment
    );
    auto image = device.createImage(imageCreateInfo);

    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();
    vk::MemoryRequirements memoryRequirements = device.getImageMemoryRequirements(image);
    uint32_t typeBits = memoryRequirements.memoryTypeBits;
    uint32_t typeIndex = uint32_t(~0);
    for ( uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++ ) {
        if ( ((typeBits & 1) != 0) && ((memoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal) ) {
            typeIndex = i;
            break;
        }
        typeBits >>= 1;
    }
    assert(typeIndex != uint32_t(~0));
    auto memory = device.allocateMemory(vk::MemoryAllocateInfo( memoryRequirements.size, typeIndex));
    device.bindImageMemory(image, memory, 0);
    vk::ComponentMapping componentMapping(
        vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA
    );
    vk::ImageSubresourceRange subResourceRange( vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1 );
    auto imageView = device.createImageView(
        vk::ImageViewCreateInfo(
            vk::ImageViewCreateFlags(),
            image,
            vk::ImageViewType::e2D,
            depthFormat,
            componentMapping,
            subResourceRange
        )
    );
    auto depthBuffer = std::shared_ptr<DepthBuffer>(new DepthBuffer{image, memory, imageView}, [&device](gsl::owner<DepthBuffer*> ptr) {
        device.destroyImageView(ptr->imageView);
        device.freeMemory(ptr->memory);
        device.destroyImage(ptr->image);
        delete ptr;
    });
    return depthBuffer;
}

vk::RenderPass Vulkan::createRenderPass(const vk::Device& device, const vk::Format& colorFormat, const vk::Format& depthFormat) {
    std::array<vk::AttachmentDescription, 2> attachmentDescriptions;
    attachmentDescriptions[0] = vk::AttachmentDescription(
        vk::AttachmentDescriptionFlags(),
        colorFormat,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR
    );
    attachmentDescriptions[1] = vk::AttachmentDescription(
        vk::AttachmentDescriptionFlags(),
        depthFormat,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eDontCare,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal
    );

    vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depthReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::SubpassDescription  subpass(
        vk::SubpassDescriptionFlags(),
        vk::PipelineBindPoint::eGraphics,
        {},
        colorReference,
        {},
        &depthReference
    );
    return device.createRenderPass(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), attachmentDescriptions, subpass));
}

std::vector<vk::Framebuffer> Vulkan::createFrameBuffers(const vk::Device& device, const std::shared_ptr<DepthBuffer>& depthBuffer, const std::shared_ptr<Swapchain>& swapchain, const vk::RenderPass& renderPass, uint32_t width, uint32_t height) {
    std::array<vk::ImageView, 2> attachments;
    attachments[1] = depthBuffer->imageView;

    vk::FramebufferCreateInfo framebufferCreateInfo(
        vk::FramebufferCreateFlags(),
        renderPass,
        attachments,
        width,
        height,
        1
    );
    std::vector<vk::Framebuffer> frameBuffers;
    frameBuffers.reserve(swapchain->imageViews.size());
    for ( auto const& imageView : swapchain->imageViews ) {
        attachments[0] = imageView;
        frameBuffers.push_back(device.createFramebuffer(framebufferCreateInfo));
    }
    return frameBuffers;
}

std::shared_ptr<Vulkan::CommandBuffer> Vulkan::createCommandBuffer(const vk::Device& device, uint32_t graphicsQueueFamilyIndex) {
    auto commandPool = device.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), graphicsQueueFamilyIndex));
    auto commandBuffers = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1));
    auto commandBuffer = commandBuffers.front();
    return std::shared_ptr<CommandBuffer>(new CommandBuffer{commandPool, commandBuffer}, [&device](gsl::owner<CommandBuffer*> ptr) {
        device.freeCommandBuffers(ptr->commandPool, ptr->commandBuffer);
        device.destroyCommandPool(ptr->commandPool);
        delete ptr;
    });
}

/*
std::shared_ptr<Vulkan::Layout> Vulkan::createLayout(const vk::Device& device) {
    vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);
    auto descriptorSetLayout = device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(), descriptorSetLayoutBinding));
    auto pipelineLayout = device.createPipelineLayout(vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), descriptorSetLayout));
    auto layout = std::shared_ptr<Layout>(new Layout{descriptorSetLayout, pipelineLayout}, [&device](gsl::owner<Layout*> ptr) {
        device.destroyPipelineLayout(ptr->pipelineLayout);
        device.destroyDescriptorSetLayout(ptr->descriptorSetLayout);
        delete ptr;
    });
    return layout;
}

std::shared_ptr<Vulkan::Descriptor> Vulkan::createDescriptor(const vk::Device& device, const vk::DescriptorSetLayout& descriptorSetLayout) {
    vk::DescriptorPoolSize poolSize( vk::DescriptorType::eUniformBuffer, 1 );
    vk::DescriptorPool descriptorPool = device.createDescriptorPool(
        vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1, poolSize)
    );

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo(descriptorPool, descriptorSetLayout);
    vk::DescriptorSet descriptorSet = device.allocateDescriptorSets(descriptorSetAllocateInfo).front();

    vk::DescriptorBufferInfo descriptorBufferInfo(uniformBufferData.buffer, 0, sizeof(glm::mat4x4));
    vk::WriteDescriptorSet writeDescriptorSet(descriptorSet, 0, 0, vk::DescriptorType::eUniformBuffer, {}, descriptorBufferInfo);
    device.updateDescriptorSets(writeDescriptorSet, nullptr);
    auto descriptor = std::shared_ptr<Descriptor>(new Descriptor{descriptorPool, descriptorSet}, [&device](gsl::owner<Descriptor*> ptr) {
        device.freeDescriptorSets(ptr->descriptorPool, ptr->descriptorSet);
        device.destroyDescriptorPool(ptr->descriptorPool);
        delete ptr;
    });
}
*/

} // namespace avk
