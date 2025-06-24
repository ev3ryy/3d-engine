#include "resource_manager.h"
#include "../loader/model_loader.h"
#include <mesh/mesh.h>
#include <material/material.h>
#include <iostream>

#include <renderer.h>
#include <vulkan/pipeline.h>

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

    auto pipeline = _renderer->getPipeline();
    size_t vertexByteOffset = pipeline->getVertexBuffer()->appendVertices(vertices);
    size_t indexByteOffset = pipeline->getIndexBuffer()->appendIndices(indices);

    mesh->vertexOffset = static_cast<uint32_t>(vertexByteOffset / sizeof(vertex));
    mesh->indexOffset = static_cast<uint32_t>(indexByteOffset / sizeof(uint32_t));
    mesh->indexCount = static_cast<uint32_t>(indices.size());
    mesh->vertexCount = static_cast<uint32_t>(vertices.size());

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

    LoadedAssetData loadedAssets = ModelLoader::Load(filepath);
    if (loadedAssets.meshes.empty()) return false;

    for (const auto& [id, material] : loadedAssets.materials) {
        RegisterMaterial(id, material);
    }

    for (const auto& [id, mesh] : loadedAssets.meshes) {
        if (m_Meshes.count(id)) continue;

        auto pipeline = _renderer->getPipeline();
        size_t vertexByteOffset = pipeline->getVertexBuffer()->appendVertices(mesh->getVertices());
        size_t indexByteOffset = pipeline->getIndexBuffer()->appendIndices(mesh->getIndices());

        mesh->vertexOffset = static_cast<uint32_t>(vertexByteOffset / sizeof(vertex));
        mesh->indexOffset = static_cast<uint32_t>(indexByteOffset / sizeof(vertex));
        mesh->vertexCount = static_cast<uint32_t>(mesh->getVertices().size());
        mesh->indexCount = static_cast<uint32_t>(mesh->getIndices().size());

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