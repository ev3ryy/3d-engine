#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "../../scene/object/object.h"

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

class Mesh;
class Material;

struct ModelData {
    std::unique_ptr<Object> rootObject;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
};

namespace ModelLoader {
    ModelData Load(const std::string& path);
}

#endif // MODEL_LOADER_H