#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

class Mesh;
class Material;

struct LoadedAssetData {
    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
};

namespace ModelLoader {
    LoadedAssetData Load(const std::string& path);

    namespace detail {
        void ProcessNode(const std::string& modelPath, aiNode* node, const aiScene* scene, LoadedAssetData& assetData, size_t& meshCounter);
        void ProcessMesh(const std::string& modelPath, aiMesh* mesh, const aiScene* scene, LoadedAssetData& assetData, size_t& meshCounter);
        void ProcessMaterial(const std::string& modelPath, aiMaterial* material, unsigned int materialIndex, LoadedAssetData& assetData);
    }
}

#endif // MODEL_LOADER_H