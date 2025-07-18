#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <mesh/mesh.h>

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <vector>
#include <utility>

#include "../../scene/object/object.h"
#include "../../scene/object/script_instance.h"

class Mesh;
class Material;
class renderer;

class IScriptInstance;

using ScriptFactoryFunc = std::unique_ptr<IScriptInstance>(*)();
struct ScriptInfo {
    std::string scriptName;
    ScriptFactoryFunc createFunc;
};
using GetScriptRegistryFunc = const std::vector<ScriptInfo>& (*)();

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

    const std::unordered_map<std::string, std::unique_ptr<Object>>& GetAllModelPrefabs() const;
    Object* GetModelPrefab(const std::string& id) const;

    void LoadAndRegisterScriptFactories(void* dllHandle);
    std::unique_ptr<IScriptInstance> CreateScriptInstance(const std::string& name);
    const std::unordered_map<std::string, ScriptFactoryFunc>& GetAllScriptFactories() const;

private:
    ResourceManager() = default;

    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_Meshes;
    std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;
    std::unordered_map<std::string, std::unique_ptr<Object>> m_ModelPrefabs;

    std::unordered_map<std::string, ScriptFactoryFunc> m_ScriptFactories;

    renderer* _renderer;
};

#endif // RESOURCE_MANAGER_H