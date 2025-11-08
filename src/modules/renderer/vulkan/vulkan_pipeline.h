#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

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

// vulkan abstraction
#include "abstract/vulkan_device.h"

// main renderer interface
#include <irenderer.h>

#include <shaders/i_shader.h>
#include <shaders/shader_manager.h>

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

struct DebugLineVertex;

struct PushConstantData {
    glm::mat4 model;
};

class VulkanPipeline : public IPipeline {
public:
    VulkanPipeline(ShaderManager* manager) : shaderManager(manager) {}
	~VulkanPipeline() = default;

    bool IsValid() override;

    // initializing
    void init() override;
    void cleanup() override;

    // draw frame
    FrameRenderStatus beginFrame() override;
    void drawFrame(RenderFrameData& frameData) override;
    FrameRenderStatus endFrame() override;

    // shader
    IShader* createShader(const ShaderBlobSet& blobs) override;

    // window
    void notifyWindowResized() override;

    // imgui
    void imguiInitialize() override;

    void uploadMesh(Mesh* mesh) override;

    void updateUniformBuffer(uint32_t currentImage, const glm::mat4& view, const glm::mat4& proj, glm::vec3 cameraPos, glm::vec3 sunDirection, float sunIntesnity);
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, const RenderFrameData& renderData, const std::vector<RenderObject>& renderObjects);

    MaterialInstance* getOrCreateMaterialInstance(Material& material);

    void createWireframeBuffers(const std::vector<DebugLineVertex>& vertices, const std::vector<uint32_t>& indices);

    VkInstance                      getInstance() const { return instance; }
    VkPhysicalDevice                getPhysicalDevice() const { return physicalDevice; }
    IDevice*                        getDevice() { return device; }
    VkQueue                         getGraphicsQueue() const { return graphicsQueue; }
    VkQueue                         getPresentQueue() const { return presentQueue; }
    uint32_t                        getQueueFamily() const { return queueFamily; }
    VkRenderPass                    getFinalRenderPass() const { return finalRenderPass; }
    VkDescriptorPool                getDescriptorPool() const { return descriptorPool; }
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

    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VulkanDevice* device = nullptr;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;

    VkCommandPool commandPool = VK_NULL_HANDLE;

    uint32_t currentFrame = 0;

    uint32_t queueFamily = 0;

    uint32_t currentSwapchainImageIndex = 0;

    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkDescriptorPool materialDescriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;

    VkDescriptorSetLayout globalDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout materialDescriptorSetLayout = VK_NULL_HANDLE;

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

    std::vector<VkSemaphore> waitSemaphoresForImage;
    std::vector<VkFence> imagesInFlight;

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_KHR_shader_non_semantic_info"
    };

    ShaderManager* shaderManager;
    std::unique_ptr<IShader> gBufferShader;

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

#endif // VULKAN_PIPELINE_H