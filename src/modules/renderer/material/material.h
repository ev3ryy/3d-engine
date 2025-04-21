#ifndef RENDERER_MATERIAL
#define RENDERER_MATERIAL

#include <string>
#include <glm/glm.hpp>

struct Material {
    std::string name;
    glm::vec3 diffuseColor = glm::vec3(1.0f);
    float ambientFactor = 0.1f; 
};

struct MaterialUniform {
    glm::vec4 diffuseColor; 
    float ambientFactor;
    glm::vec3 pad;
    glm::vec4 sunParameters;
};

inline static MaterialUniform ConvertMaterial(const Material& mat) {
    MaterialUniform out = {};
    out.diffuseColor = glm::vec4(mat.diffuseColor, 1.0f);
    out.ambientFactor = mat.ambientFactor;
    out.pad = glm::vec3(0.0f);
    out.sunParameters = glm::vec4(glm::normalize(glm::vec3(1.0f, 1.0f, -1.0f)), 2.0f);
    return out;
}

#endif // RENDERER_MATERIAL
