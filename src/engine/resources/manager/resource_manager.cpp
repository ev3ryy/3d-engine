#include "resource_manager.h"
#include "../loader/model_loader.h"
#include <mesh/mesh.h>
#include <material/material.h>
#include <iostream>

#include <renderer.h>
#include <vulkan/pipeline.h>

#include <logs.h>

#ifdef _WIN32
#include <Windows.h>
#define GET_FUNC GetProcAddress
#else
#include <dlfcn.h>
#define GET_FUNC dlsym
#endif

ResourceManager& ResourceManager::Get() {
    static ResourceManager instance;
    return instance;
}

void ResourceManager::Initialize(renderer* renderer)
{
    _renderer = renderer;
}

bool ResourceManager::CreatePrimitive(const std::string& name, std::function<std::pair<std::vector<vertex>, std::vector<uint32_t>>()> generator)
{
    if (!_renderer) return false;

    auto [vertices, indices] = generator();
    auto mesh = std::make_shared<Mesh>(vertices, indices);

    std::string materialId = name + "_Mat";
    mesh->materialId_ = materialId;

    RegisterMesh(name, mesh);

    if (m_Materials.count(materialId)) {
        return true;
    }

    auto matDef = std::make_shared<Material>();
    matDef->name = materialId;
    matDef->shaderName = "PBR_Opaque";
    RegisterMaterial(materialId, matDef);

    return true;
}

bool ResourceManager::LoadModel(const std::string& filepath) {
    if (!_renderer) return false;

    ModelData loadedModel = ModelLoader::Load(filepath);
    if (!loadedModel.rootObject) return false;

    for (const auto& [id, mesh] : loadedModel.meshes) {
        RegisterMesh(id, mesh);
    }
    for (const auto& [id, material] : loadedModel.materials) {
        RegisterMaterial(id, material);
    }

    m_ModelPrefabs[filepath] = std::move(loadedModel.rootObject);
    return true;
}

const std::unordered_map<std::string, std::unique_ptr<Object>>& ResourceManager::GetAllModelPrefabs() const {
    return m_ModelPrefabs;
}

Object* ResourceManager::GetModelPrefab(const std::string& id) const {
    auto it = m_ModelPrefabs.find(id);
    if (it != m_ModelPrefabs.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::shared_ptr<Mesh> ResourceManager::GetMesh(const std::string& id) {
    if (m_Meshes.count(id)) {
        return m_Meshes.at(id);
    }
    return nullptr;
}

std::shared_ptr<Material> ResourceManager::GetMaterial(const std::string& id) {
    if (m_Materials.count(id)) {
        return m_Materials.at(id);
    }
    return nullptr;
}

void ResourceManager::RegisterMesh(const std::string& id, std::shared_ptr<Mesh> mesh) {
    if (!m_Meshes.count(id)) {
        m_Meshes[id] = mesh;
    }
}

void ResourceManager::RegisterMaterial(const std::string& id, std::shared_ptr<Material> material) {
    if (!m_Materials.count(id)) {
        m_Materials[id] = material;
    }
}

void ResourceManager::LoadAndRegisterScriptFactories(void* dllHandle)
{
    if (!dllHandle) {
        LOG_ERROR("ResourceManager: Попытка зарегистрировать скрипты из невалидного хэндла DLL.");
        return;
    }

    GetScriptRegistryFunc getRegistry = (GetScriptRegistryFunc)GET_FUNC((HMODULE)dllHandle, "GetScriptRegistry");
    if (!getRegistry) {
        LOG_ERROR("ResourceManager: Не удалось найти функцию 'GetScriptRegistry' в предоставленной DLL.");
        return;
    }

    const auto& registry = getRegistry();
    for (const auto& info : registry) {
        if (m_ScriptFactories.find(info.scriptName) == m_ScriptFactories.end()) {
            m_ScriptFactories[info.scriptName] = info.createFunc;
            LOG_INFO("ResourceManager: Зарегистрирована фабрика для скрипта '%s'.", info.scriptName.c_str());
        }
    }
}

std::unique_ptr<IScriptInstance> ResourceManager::CreateScriptInstance(const std::string& name)
{
    auto it = m_ScriptFactories.find(name);
    if (it != m_ScriptFactories.end()) {
        return it->second();
    }

    LOG_ERROR("ResourceManager: Не удалось найти фабрику для создания скрипта с именем '%s'.", name.c_str());
    return nullptr;
}

const std::unordered_map<std::string, ScriptFactoryFunc>& ResourceManager::GetAllScriptFactories() const
{
    return m_ScriptFactories;
}