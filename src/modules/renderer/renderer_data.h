#ifndef RENDERER_DATA_H
#define RENDERER_DATA_H

#include <glm/glm.hpp>
#include <vector>
#include "../physics/utils/drawer.h"
#include <imgui.h>

struct RenderObject {
    MaterialInstance* material = nullptr;
    Mesh* mesh = nullptr;
    glm::mat4 modelMatrix;
};

struct RenderItem {
    glm::mat4 modelMatrix;
    MaterialInstance material;
    uint32_t indexCount;
    uint32_t indexOffset;
    uint32_t vertexOffset;
};

struct RenderFrameData {
    RenderFrameData(
        std::vector<RenderObject>& inRenderObjects,
        const std::vector<DebugLineVertex>& inPhysicsVertices,
        const std::vector<uint32_t>& inPhysicsIndices
    ) :
        renderObjects(inRenderObjects),
        physicsDebugVertices(inPhysicsVertices),
        physicsDebugIndices(inPhysicsIndices)
    {
    }

    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;
    VkDescriptorSet globalDescriptorSet = VK_NULL_HANDLE;

    float cameraFov;
    float cameraNearPlane;
    float cameraFarPlane;
    glm::vec3 cameraPosition;

    const std::vector<DebugLineVertex>& physicsDebugVertices;
    const std::vector<uint32_t>& physicsDebugIndices;

    std::vector<RenderObject>& renderObjects;
    std::vector<RenderItem> renderItems;

    uint32_t viewportWidth;
    uint32_t viewportHeight;

    ImVec4 clearColor;
    ImDrawData* imguiDrawData = nullptr;
};

enum class FrameRenderStatus {
    Success,
    Error,
    SwapChainNeedsResize
};

#endif // RENDERER_DATA_H