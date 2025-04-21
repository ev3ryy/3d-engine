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
    glm::vec3 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

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

        return attributeDescriptions;
    }
};

struct Transform {
    glm::vec3 translation = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, translation);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        return model;
    }
};

struct InstanceGroup {
    mesh* meshPtr;
    std::vector<Transform> instances;
};

struct InstanceData {
    glm::mat4 model;
    glm::vec3 color;
    float _padding;
};

struct UniformBufferObject {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 sunLightDirection;
    float sunLightIntensity;
};

class mesh {
public:
    mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices)
        : vertices_(vertices), indices_(indices), indexCount(static_cast<uint32_t>(indices.size()))
    {
    }

    const std::vector<vertex>& getVertices() const { return vertices_; }
    const std::vector<uint32_t>& getIndices() const { return indices_; }

    static VkVertexInputBindingDescription getBindingDescription() {
        return vertex::getBindingDescription();
    }
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        return vertex::getAttributeDescriptions();
    }

    Transform transform;

    uint32_t vertexOffset = 0;
    uint32_t indexOffset = 0;
    uint32_t indexCount = 0;

    Material material;

private:
    std::vector<vertex> vertices_;
    std::vector<uint32_t> indices_;
};


#endif // RENDERER_MESH_H