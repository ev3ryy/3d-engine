#ifndef RENDERER_MESH_H
#define RENDERER_MESH_H

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <material/material.h>

#include <vector>
#include <array>
#include <cstddef>
#include <cstdint>

class mesh;

struct vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

        // 0: position
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(vertex, pos);

        // 1: normal map
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(vertex, normal);

        // 2: UV coords
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(vertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(vertex, tangent);

        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(vertex, bitangent);

        return attributeDescriptions;
    }
};

struct DebugLineVertex {
    glm::vec3 position;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(DebugLineVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(DebugLineVertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(DebugLineVertex, color);

        return attributeDescriptions;
    }
};

struct UniformBufferObject {
    glm::mat4 view;
    glm::mat4 proj;

    glm::mat4 invProj;
    glm::mat4 invView;

    glm::vec3 sunLightDirection;
    float sunLightIntensity;
    alignas(16) glm::vec3 cameraPosition;
};

class Mesh {
public:
    Mesh() = default;
    Mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices)
        : vertices_(vertices), indices_(indices), indexCount(static_cast<uint32_t>(indices.size()))
    {

    }
    ~Mesh() {};

    uint32_t materialIndex_ = 0;
    std::string materialId_;

    const std::vector<vertex>& getVertices() const { return vertices_; }
    const std::vector<uint32_t>& getIndices() const { return indices_; }

    static VkVertexInputBindingDescription getBindingDescription() {
        return vertex::getBindingDescription();
    }
    static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() {
        return vertex::getAttributeDescriptions();
    }

    uint32_t vertexOffset = 0;
    uint32_t indexOffset = 0;

    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;

//private:
    std::vector<vertex> vertices_;
    std::vector<uint32_t> indices_;
};


#endif // RENDERER_MESH_H