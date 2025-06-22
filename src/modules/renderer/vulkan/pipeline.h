#ifndef RENDERER_PIPELINE_H
#define RENDERER_PIPELINE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <vector>
#include <optional>
#include <array>
#include <memory>
#include <unordered_map>

#include "buffers.h"
#include "queuefamily.h"
#include "swapchain.h"
#include "validation.h"

#include "mesh/mesh.h"

#include "window/window.h"

#include <imgui.h>

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

struct PushConstantData {
    glm::mat4 model;
    MaterialUniform material;
};

struct RenderItem {
    glm::mat4 modelMatrix;
    MaterialUniform material;
    uint32_t indexCount;
    uint32_t indexOffset;
    uint32_t vertexOffset;
};

struct RenderFrameData {
    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;
    VkDescriptorSet globalDescriptorSet = VK_NULL_HANDLE;
    std::vector<RenderItem> renderItems;

    uint32_t viewportWidth;
    uint32_t viewportHeight;

    ImVec4 clearColor;
    ImDrawData* imguiDrawData = nullptr;
};

class pipeline {
public:
    pipeline();
	~pipeline();

    void updateUniformBuffer(uint32_t currentImage, const glm::mat4& view, const glm::mat4& proj);
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, const RenderFrameData& renderData);

    VkInstance                      getInstance() const { return instance; }
    VkPhysicalDevice                getPhysicalDevice() const { return physicalDevice; }
    VkDevice                        getDevice() const { return device; }
    VkQueue                         getGraphicsQueue() const { return graphicsQueue; }
    VkQueue                         getPresentQueue() const { return presentQueue; }
    uint32_t                        getQueueFamily() const { return queueFamily; }
    VkRenderPass                    getRenderPass() const { return renderPass; }
    VkDescriptorPool                getDescriptorPool() const { return descriptorPool; }
    uint32_t                        getMinImageCount() const { return _swapchain->minImageCount; }
    uint32_t                        getImageCount() const { return _swapchain->imageCount; }
    uint32_t                        getCurrentFrame() const { return currentFrame; }
    VkImageView                     getDepthImageView() const { return depthImageView; };
    std::vector<VkDescriptorSet>    getDescriptorSets() const { return descriptorSets; }

    void                            setCurrentFrame(uint32_t currentFrame) { currentFrame = currentFrame; }
    
    swapchain* getSwapchain() const { return _swapchain; }

    buffers::vertexBuffer* getVertexBuffer() const { return _vertexBuffer; }
    buffers::indexBuffer* getIndexBuffer() const { return _indexBuffer; }

    bool vsync = false;

    ImVec4 imClearColor;

    std::vector<Mesh> meshes;
    std::unordered_map<Mesh*, InstanceGroup> instanceGroups;

    std::vector<VkFence> inFlightFences;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;

    std::vector<VkCommandBuffer> commandBuffers;

private:
	void init();
	void cleanup();

    void createInstance();
    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    void createLogicalDevice();
    void createSurface();
    bool checkDeviceExtensionsSupport(VkPhysicalDevice device);

    void createImage(uint32_t width, uint32_t height, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image, VkDeviceMemory& imageMemory);
    VkFormat findDepthFormat();
    void createDepthResources();
    void createRenderPass();
    void createGraphicsPipeline();
    VkShaderModule createShaderModule(const std::vector<char>& code);

    void createCommandPool();
    void createCommandBuffer();
    void createSyncObjects();

    void createDescriptorPool();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorSets();

    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;


    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;

    uint32_t currentFrame = 0;

    uint32_t queueFamily = 0;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    VkDescriptorSetLayout descriptorSetLayout;

    //VkBuffer vertexBuffer; // vertices
    //VkDeviceMemory vertexBufferMemory;
    //VkBuffer indexBuffer; // indices
    //VkDeviceMemory indexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    std::vector<VkBuffer> materialUniformBuffers;
    std::vector<VkDeviceMemory> materialUniformBuffersMemory;
    std::vector<void*> materialUniformBuffersMapped;

    std::vector<VkBuffer> modelUniformBuffers;
    std::vector<VkDeviceMemory> modelUniformBuffersMemory;
    std::vector<VkDescriptorSet> modelDescriptorSets;

    size_t currentInstanceCapacity = 100;

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    buffers::vertexBuffer* _vertexBuffer;
    buffers::indexBuffer* _indexBuffer;
    swapchain* _swapchain;

    VmaAllocator allocator;

    PushConstantData pushData;

    VkImage depthImage;
    VkImageView depthImageView;
    VkDeviceMemory depthImageMemory;
};

#endif // RENDERER_VULKAN_H