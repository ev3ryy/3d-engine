// Material.h
#ifndef RENDERER_MATERIAL
#define RENDERER_MATERIAL

#include <vulkan/buffers/uniform_buffer.h>
#include <string>
#include <glm/glm.hpp>

struct MaterialData {
    alignas(16) glm::vec4 albedoColor;
    alignas(16) glm::vec4 pbrParams;
    // pbrParams.x = metallic
    // pbrParams.y = roughness
    // pbrParams.z = ambientOcclusion
    // pbrParams.w = padding for alignment

    alignas(16) glm::ivec4 textureFlags;
    // textureFlags.x = hasAlbedoMap
    // textureFlags.y = hasNormalMap
    // textureFlags.z = hasMetallicRoughessMap
    // textureFlags.w = hasAoMap
};

class Material {
public:
    std::string name;
    std::string shaderName;

    glm::vec4 albedoColor = glm::vec4(1.0f);
    float metallic = 0.0f;
    float roughness = 1.0f;
    float ambientOcclusion = 1.0f;

    std::string albedoPath;
    std::string normalMapPath;
    std::string metallicRoughnessPath;
    std::string aoMapPath;

    bool isDirty = true;

    MaterialData toMaterialData() const {
        MaterialData data;
        data.albedoColor = albedoColor;
        data.pbrParams = glm::vec4(metallic, roughness, ambientOcclusion, 0.0f);
        data.textureFlags = glm::ivec4(!albedoPath.empty(), !normalMapPath.empty(), !metallicRoughnessPath.empty(), !aoMapPath.empty());
        return data;
    }
};

class MaterialInstance {
public:
    MaterialInstance() : material(nullptr) {}
    ~MaterialInstance() { material = nullptr;  }

    Material* material;
    std::unique_ptr<UniformBuffer> buffer;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};

#endif // RENDERER_MATERIAL