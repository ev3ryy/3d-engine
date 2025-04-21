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
#include "camera/camera.h"

#include <imgui.h>

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

struct PushConstantData {
    glm::mat4 model;
    MaterialUniform material;
};

class pipeline {
public:
    pipeline();
	~pipeline();

    void drawFrame();

    void addInstance(mesh* meshPtr, const Transform& transform);

    VkInstance          getInstance() const { return instance; }
    VkPhysicalDevice    getPhysicalDevice() const { return physicalDevice; }
    VkDevice            getDevice() const { return device; }
    VkQueue             getGraphicsQueue() const { return graphicsQueue; }
    uint32_t            getQueueFamily() const { return queueFamily; }
    VkRenderPass        getRenderPass() const { return renderPass; }
    VkDescriptorPool    getDescriptorPool() const { return descriptorPool; }
    uint32_t            getMinImageCount() const { return _swapchain->minImageCount; }
    uint32_t            getImageCount() const { return _swapchain->imageCount; }
    
    swapchain* getSwapchain() const { return _swapchain; }

    buffers::vertexBuffer* getVertexBuffer() const { return _vertexBuffer; }
    buffers::indexBuffer* getIndexBuffer() const { return _indexBuffer; }

    bool vsync = false;

    camera _camera;

    ImVec4 imClearColor;

    std::vector<mesh> meshes;
    std::unordered_map<mesh*, InstanceGroup> instanceGroups;

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

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void createSyncObjects();

    void createDescriptorPool();
    //void createVertexBuffer(const std::vector<mesh>& meshes);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    //void createIndexBuffer();
    void createDescriptorSetLayout();
    void createUniformBuffers();
    void updateUniformBuffer(uint32_t currentImage);
    void createMaterialUniformBuffers();
    void updateMaterialUniformBuffer(uint32_t currentImage, const MaterialUniform& materialData);
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
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
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