#include "model_loader.h"
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh/mesh.h>
#include <material/material.h>

#include <glm/glm.hpp>

namespace ModelLoader {

    LoadedAssetData Load(const std::string& path) {
        LoadedAssetData assetData;
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_FlipUVs |
            aiProcess_FlipWindingOrder);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return assetData;
        }

        size_t meshCounter = 0;
        detail::ProcessNode(path, scene->mRootNode, scene, assetData, meshCounter);
        return assetData;
    }

    namespace detail {

        void ProcessNode(const std::string& modelPath, aiNode* node, const aiScene* scene, LoadedAssetData& assetData, size_t& meshCounter) {
            for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                ProcessMesh(modelPath, mesh, scene, assetData, meshCounter);
            }

            for (unsigned int i = 0; i < node->mNumChildren; i++) {
                ProcessNode(modelPath, node->mChildren[i], scene, assetData, meshCounter);
            }
        }

        void ProcessMesh(const std::string& modelPath, aiMesh* mesh, const aiScene* scene, LoadedAssetData& assetData, size_t& meshCounter) {
            auto engineMesh = std::make_shared<Mesh>();

            engineMesh->vertices_.resize(mesh->mNumVertices);
            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                vertex& vertex = engineMesh->vertices_[i];
                vertex.pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
                if (mesh->HasNormals()) {
                    vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
                }
                if (mesh->mTextureCoords[0]) {
                    vertex.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
                }
                else {
                    vertex.texCoord = { 0.0f, 0.0f };
                }

                if (mesh->HasTangentsAndBitangents()) {
                    vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
                    vertex.bitangent = { mesh->mBitangents[i].y, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
                }
                else {
                    vertex.tangent = { 0.0f, 0.0f, 0.0f };
                    vertex.bitangent = { 0.0f, 0.0f, 0.0f };
                }
            }

            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    engineMesh->indices_.push_back(face.mIndices[j]);
                }
            }

            engineMesh->materialIndex_ = mesh->mMaterialIndex;

            ProcessMaterial(modelPath, scene->mMaterials[mesh->mMaterialIndex], mesh->mMaterialIndex, assetData);

            engineMesh->materialId_ = modelPath + "/mat_" + std::to_string(mesh->mMaterialIndex);

            std::string meshId = modelPath + "/" + (mesh->mName.length > 0 ? mesh->mName.C_Str() : "mesh_" + std::to_string(meshCounter++));

            assetData.meshes[meshId] = engineMesh;

            std::cout << "Loaded Mesh: " << meshId << " with " << engineMesh->vertices_.size() << " vertices." << std::endl;
        }

        void ProcessMaterial(const std::string& modelPath, aiMaterial* material, unsigned int materialIndex, LoadedAssetData& assetData) {
            std::string materialId = modelPath + "/mat_" + std::to_string(materialIndex);

            if (assetData.materials.count(materialId)) {
                return;
            }

            auto engineMaterial = std::make_shared<Material>();
            engineMaterial->name = materialId;

            aiColor4D color;
            if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
                engineMaterial->albedoColor = glm::vec4(color.r, color.g, color.b, color.a);
            }

            assetData.materials[materialId] = engineMaterial;
            std::cout << "Loaded Material: " << materialId << std::endl;
        }

    }
}