#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <mesh/mesh.h>

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <vector>
#include <utility>

class Mesh;
class Material;
class renderer;

class ResourceManager {
public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    static ResourceManager& Get();

    void Initialize(renderer* renderer);

    bool CreatePrimitive(const std::string& name, std::function<std::pair<std::vector<vertex>, std::vector<uint32_t>>()> generator);

    bool LoadModel(const std::string& filepath);

    std::shared_ptr<Mesh> GetMesh(const std::string& id);
    std::shared_ptr<Material> GetMaterial(const std::string& id);

    const std::unordered_map<std::string, std::shared_ptr<Mesh>>& GetAllMeshes() const { return m_Meshes; }
    const std::unordered_map<std::string, std::shared_ptr<Material>>& GetAllMaterials() const { return m_Materials; }

    void RegisterMesh(const std::string& id, std::shared_ptr<Mesh> mesh);
    void RegisterMaterial(const std::string& id, std::shared_ptr<Material> material);

private:
    ResourceManager() = default;

    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_Meshes;
    std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;

    renderer* _renderer;
};

#endif // RESOURCE_MANAGER_H