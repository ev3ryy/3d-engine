#ifndef RENDERER_VULKAN_QUEUEFAMILIES
#define RENDERER_VULKAN_QUEUEFAMILIES

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

namespace QueueFamily {
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
}

#endif // RENDERER_VULKAN_QUEUEFAMILIES