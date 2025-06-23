#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <string>
#include <unordered_map>
#include <memory>

class Mesh;
class Material;

class ResourceManager {
public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    static ResourceManager& Get();

    bool LoadModel(const std::string& filepath);

    std::shared_ptr<Mesh> GetMesh(const std::string& id);
    //std::shared_ptr<Material> GetMaterial(const std::string& id);

    const std::unordered_map<std::string, std::shared_ptr<Mesh>>& GetAllMeshes() const { return m_Meshes; }
    const std::unordered_map<std::string, std::shared_ptr<Material>>& GetAllMaterials() const { return m_Materials; }

    void RegisterMesh(const std::string& id, std::shared_ptr<Mesh> mesh);
    //void RegisterMaterial(const std::string& id, std::shared_ptr<Material> material);

private:
    ResourceManager() = default;

    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_Meshes;
    std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;
};

#endif // RESOURCE_MANAGER_H