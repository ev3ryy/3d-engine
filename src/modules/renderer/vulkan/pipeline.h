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

#include "buffers/buffers.h"
#include "queuefamily.h"
#include "swapchain.h"
#include "validation.h"

#include "mesh/mesh.h"

#include "window/window.h"

#include <imgui.h>

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

struct DebugLineVertex;

struct PushConstantData {
    glm::mat4 model;
};

struct RenderItem {
    glm::mat4 modelMatrix;
    MaterialInstance material;
    uint32_t indexCount;
    uint32_t indexOffset;
    uint32_t vertexOffset;
};

struct RenderObject {
    MaterialInstance* material = nullptr;
    Mesh* mesh = nullptr;
    glm::mat4 modelMatrix;
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

    void updateUniformBuffer(uint32_t currentImage, const glm::mat4& view, const glm::mat4& proj, glm::vec3 cameraPos);
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, const RenderFrameData& renderData, const std::vector<RenderObject>& renderObjects);

    MaterialInstance* getOrCreateMaterialInstance(Material& material);

    void createWireframeBuffers(const std::vector<DebugLineVertex>& vertices, const std::vector<uint32_t>& indices);

    VkInstance                      getInstance() const { return instance; }
    VkPhysicalDevice                getPhysicalDevice() const { return physicalDevice; }
    VkDevice                        getDevice() const { return device; }
    VkQueue                         getGraphicsQueue() const { return graphicsQueue; }
    VkQueue                         getPresentQueue() const { return presentQueue; }
    uint32_t                        getQueueFamily() const { return queueFamily; }
    //VkRenderPass                    getLightingRenderPass() const { return lightingRenderPass; }
    //VkRenderPass                    getImGuiRenderPass() const { return imguiRenderPass; }
    VkRenderPass                    getFinalRenderPass() const { return finalRenderPass; }
    VkDescriptorPool                getDescriptorPool() const { return descriptorPool; }
    uint32_t                        getMinImageCount() const { return _swapchain->minImageCount; }
    uint32_t                        getImageCount() const { return _swapchain->imageCount; }
    uint32_t                        getCurrentFrame() const { return currentFrame; }
    VkImageView                     getSwapchainDepthImageView() const { return swapchainDepthImageView; };
    std::vector<VkDescriptorSet>    getDescriptorSets() const { return descriptorSets; }

    void                            setCurrentFrame(uint32_t currentFrame) { currentFrame = currentFrame; }
    
    swapchain* getSwapchain() const { return _swapchain; }

    buffers::vertexBuffer* getVertexBuffer() const { return _vertexBuffer; }
    buffers::indexBuffer* getIndexBuffer() const { return _indexBuffer; }

    bool vsync = false;

    ImVec4 imClearColor;

    std::vector<VkFence> inFlightFences;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;

    std::vector<VkCommandBuffer> commandBuffers;

    glm::vec3 sunDirection = glm::vec3(1.0f, -1.0f, 1.0f);
    float sunIntesnity = 50.0f;

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

    void createGBufferRenderPass();
    //void createLightingRenderPass();
    //void createWireframeRenderPass();
    //void createImGuiRenderPass();

    void createFinalRenderPass();

    void createGBufferPipeline();
    void createLightingPipeline();
    void createWireframePipeline();

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
    void createGlobalDescriptorSet();
    void createMaterialDescriptorPool();

    void createGBufferFramebuffer();
    void createGBufferDescriptorSetLayout();
    void createGBufferDescriptorSet();

    void createGBufferResources();
    void createGBufferSampler();

    void createSwapchainDepthResources();

    void destroyWireframeBuffers();

    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;

    uint32_t currentFrame = 0;

    uint32_t queueFamily = 0;

    VkDescriptorPool descriptorPool;
    VkDescriptorPool materialDescriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    VkDescriptorSetLayout globalDescriptorSetLayout;
    VkDescriptorSetLayout materialDescriptorSetLayout;

    //VkBuffer vertexBuffer; // vertices
    //VkDeviceMemory vertexBufferMemory;
    //VkBuffer indexBuffer; // indices
    //VkDeviceMemory indexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    //std::vector<VkBuffer> materialUniformBuffers;
    //std::vector<VkDeviceMemory> materialUniformBuffersMemory;
    //std::vector<void*> materialUniformBuffersMapped;

    std::vector<VkBuffer> modelUniformBuffers;
    std::vector<VkDeviceMemory> modelUniformBuffersMemory;
    std::vector<VkDescriptorSet> modelDescriptorSets;

    size_t currentInstanceCapacity = 100;

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    buffers::vertexBuffer* _vertexBuffer;
    buffers::indexBuffer* _indexBuffer;

    VkBuffer wireframeVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory wireframeVertexMemory = VK_NULL_HANDLE;
    VkBuffer wireframeIndexBuffer = VK_NULL_HANDLE; 
    VkDeviceMemory wireframeIndexMemory = VK_NULL_HANDLE;

    uint32_t wireframeVertexCount = 0;
    uint32_t wireframeIndexCount = 0;

    swapchain* _swapchain;

    VmaAllocator allocator;

    PushConstantData pushData;

    //VkImage depthImage;
    //VkImageView depthImageView;
    //VkDeviceMemory depthImageMemory;

    VkImage gBufferDepthImage = VK_NULL_HANDLE;
    VkDeviceMemory gBufferDepthImageMemory = VK_NULL_HANDLE;
    VkImageView gBufferDepthImageView = VK_NULL_HANDLE;

    VkImage swapchainDepthImage = VK_NULL_HANDLE;
    VkDeviceMemory swapchainDepthImageMemory = VK_NULL_HANDLE;
    VkImageView swapchainDepthImageView = VK_NULL_HANDLE;

    std::unordered_map<std::string, std::unique_ptr<MaterialInstance>> materialCache;

    // gBuffer setup
    struct {
        VkImage albedo;
        VkDeviceMemory albedoMem;
        VkImageView albedoView;
        VkImage normal;
        VkDeviceMemory normalMem;
        VkImageView normalView;
        VkImage emissive;
        VkDeviceMemory emissiveMem;
        VkImageView emissiveView;
    } gBuffer;

    VkFramebuffer gBufferFramebuffer;

    VkRenderPass gBufferRenderPass;
    //VkRenderPass lightingRenderPass;
    //VkRenderPass wireframeRenderPass;
    //VkRenderPass imguiRenderPass;

    VkRenderPass finalRenderPass;

    VkPipeline gBufferPipeline;
    VkPipelineLayout gBufferPipelineLayout;

    VkPipeline lightingPipeline;
    VkPipelineLayout lightingPipelineLayout;

    VkPipeline wireframePipeline;
    VkPipelineLayout wireframePipelineLayout;

    VkSampler gBufferSampler;
    VkDescriptorSetLayout gBufferDescriptorSetLayout;
    VkDescriptorSet gBufferDescriptorSet;

    // delete this
    VkImage defaultAlbedoImage = VK_NULL_HANDLE;
    VmaAllocation defaultAlbedoImageAllocation = nullptr;
    VkImageView defaultAlbedoImageView = VK_NULL_HANDLE;
    VkSampler defaultAlbedoSampler = VK_NULL_HANDLE;

    VkImage defaultNormalImage = VK_NULL_HANDLE;
    VmaAllocation defaultNormalImageAllocation = nullptr;
    VkImageView defaultNormalImageView = VK_NULL_HANDLE;
    VkSampler defaultNormalSampler = VK_NULL_HANDLE;

    VkImage defaultMetallicRoughnessImage = VK_NULL_HANDLE;
    VmaAllocation defaultMetallicRoughnessImageAllocation = nullptr;
    VkImageView defaultMetallicRoughnessImageView = VK_NULL_HANDLE;
    VkSampler defaultMetallicRoughnessSampler = VK_NULL_HANDLE;

    VkImage defaultAoImage = VK_NULL_HANDLE;
    VmaAllocation defaultAoImageAllocation = nullptr;
    VkImageView defaultAoImageView = VK_NULL_HANDLE;
    VkSampler defaultAoSampler = VK_NULL_HANDLE;


    void createDefaultTextures();
    void cleanupDefaultTextures();

    void createSingleDefaultTexture(
        uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage,
        VkImage& image, VmaAllocation& imageAllocation, VkImageView& imageView,
        const std::vector<unsigned char>& pixelData
    );
    void createDefaultSampler(VkSampler& sampler);

    VkDescriptorImageInfo GetDefaultAlbedoTextureInfo() const;
    VkDescriptorImageInfo GetDefaultNormalTextureInfo() const;
    VkDescriptorImageInfo GetDefaultMetallicRoughnessTextureInfo() const;
    VkDescriptorImageInfo GetDefaultAoTextureInfo() const;

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandBuffer commandBuffer);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandBuffer commandBuffer);
};

#endif // RENDERER_VULKAN_H