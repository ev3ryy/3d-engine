#include "resource_manager.h"
#include "../loader/model_loader.h"
#include <mesh/mesh.h>
#include <material/material.h>
#include <iostream>

ResourceManager& ResourceManager::Get() {
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::LoadModel(const std::string& filepath) {
    LoadedAssetData loadedAssets = ModelLoader::Load(filepath);

    if (loadedAssets.meshes.empty() && loadedAssets.materials.empty()) {
        return false;
    }

    //for (const auto& [id, material] : loadedAssets.materials) {
    //    RegisterMaterial(id, material);
    //}

    for (const auto& [id, mesh] : loadedAssets.meshes) {
        RegisterMesh(id, mesh);
    }

    return true;
}

std::shared_ptr<Mesh> ResourceManager::GetMesh(const std::string& id) {
    if (m_Meshes.count(id)) {
        return m_Meshes.at(id);
    }
    return nullptr;
}

//std::shared_ptr<Material> ResourceManager::GetMaterial(const std::string& id) {
//    if (m_Materials.count(id)) {
//        return m_Materials.at(id);
//    }
//    return nullptr;
//}

void ResourceManager::RegisterMesh(const std::string& id, std::shared_ptr<Mesh> mesh) {
    if (!m_Meshes.count(id)) {
        m_Meshes[id] = mesh;
    }
}

//void ResourceManager::RegisterMaterial(const std::string& id, std::shared_ptr<Material> material) {
//    if (!m_Materials.count(id)) {
//        m_Materials[id] = material;
//    }
//}