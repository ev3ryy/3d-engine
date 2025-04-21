#ifndef RENDERER_VULKAN_SWAPCHAIN
#define RENDERER_VULKAN_SWAPCHAIN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <stdexcept>

#include "buffers.h"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class swapchain {
public:
	swapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);
	~swapchain();

    void createSwapChain();
    void recreateSwapChain(VkRenderPass renderPass, VkImageView depthImageView);
    void cleanupSwapChain();

    void createImageViews();

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

    void createFramebuffers(VkRenderPass renderPass, VkImageView depthImageView);

    VkFramebuffer getSwapchainFramebuffer(uint32_t imageIndex) const {
        if (imageIndex >= swapChainFramebuffers.size()) {
            throw std::runtime_error("lol");
        }
        return swapChainFramebuffers[imageIndex];
    }

    std::vector<VkFramebuffer> getSwapchainFramebuffer() const { return swapChainFramebuffers; }

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    uint32_t minImageCount = 2;
    uint32_t imageCount;

private:
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
};

#endif // RENDERER_VULKAN_SWAPCHAIN