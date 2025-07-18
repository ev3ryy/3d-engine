#include "model_loader.h"
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include <logs.h>
#include "mesh/mesh.h"
#include "material/material.h"

static inline glm::mat4 AiMatrix4x4ToGlm(const aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

namespace ModelLoader {
    namespace detail {
        std::unique_ptr<Object> ProcessNode(aiNode* node, const aiScene* scene, ModelData& modelData, const std::string& modelPath);
        std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, ModelData& modelData, const std::string& modelPath, const std::string& meshName);
        std::shared_ptr<Material> ProcessMaterial(aiMaterial* material, ModelData& modelData, const std::string& modelPath, unsigned int materialIndex);

        std::unique_ptr<Object> ProcessNode(aiNode* node, const aiScene* scene, ModelData& modelData, const std::string& modelPath) {
            auto newObject = std::make_unique<Object>(node->mName.C_Str());

            TransformComponent* tc = newObject->getTransform();
            if (tc) {
                glm::mat4 transform = AiMatrix4x4ToGlm(node->mTransformation);
                glm::vec3 skew;
                glm::vec4 perspective;
                glm::quat rotationQuat;

                glm::decompose(transform, tc->scale, rotationQuat, tc->position, skew, perspective);

                tc->setRotation(rotationQuat);
            }

            for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
                std::string meshName = ai_mesh->mName.length > 0 ? ai_mesh->mName.C_Str() : (std::string(node->mName.C_Str()) + "_mesh_" + std::to_string(i));

                std::shared_ptr<Mesh> engineMesh = ProcessMesh(ai_mesh, scene, modelData, modelPath, meshName);
                if (engineMesh) {
                    auto meshObject = std::make_unique<Object>(meshName);
                    meshObject->addComponent<MeshRendererComponent>(engineMesh, engineMesh->materialId_);
                    newObject->addChild(std::move(meshObject));
                }
            }

            for (unsigned int i = 0; i < node->mNumChildren; i++) {
                newObject->addChild(ProcessNode(node->mChildren[i], scene, modelData, modelPath));
            }

            return newObject;
        }

        std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, ModelData& modelData, const std::string& modelPath, const std::string& meshName) {
            std::string meshId = modelPath + "/" + meshName;
            if (modelData.meshes.count(meshId)) {
                return modelData.meshes[meshId];
            }

            auto engineMesh = std::make_shared<Mesh>();
            engineMesh->vertices_.resize(mesh->mNumVertices);
            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                vertex& v = engineMesh->vertices_[i];
                v.pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
                if (mesh->HasNormals()) v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
                if (mesh->mTextureCoords[0]) v.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
                if (mesh->HasTangentsAndBitangents()) {
                    v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
                    v.bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
                }
            }
            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                    engineMesh->indices_.push_back(face.mIndices[j]);
            }

            if (mesh->mMaterialIndex >= 0) {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                std::shared_ptr<Material> engineMaterial = ProcessMaterial(material, modelData, modelPath, mesh->mMaterialIndex);
                engineMesh->materialId_ = engineMaterial->name;
            }

            modelData.meshes[meshId] = engineMesh;
            LOG_INFO("Processed Mesh: %s", meshId.c_str());
            return engineMesh;
        }

        std::shared_ptr<Material> ProcessMaterial(aiMaterial* material, ModelData& modelData, const std::string& modelPath, unsigned int materialIndex) {
            std::string materialId = modelPath + "_Mat_" + std::to_string(materialIndex);
            if (modelData.materials.count(materialId)) {
                return modelData.materials[materialId];
            }

            auto engineMaterial = std::make_shared<Material>();
            engineMaterial->name = materialId;
            engineMaterial->shaderName = "PBR_Opaque";

            aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
            if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
                engineMaterial->albedoColor = glm::vec4(color.r, color.g, color.b, color.a);
            }
            else if (aiGetMaterialColor(material, AI_MATKEY_BASE_COLOR, &color) == AI_SUCCESS) {
                engineMaterial->albedoColor = glm::vec4(color.r, color.g, color.b, color.a);
            }

            float metallic = 0.0f;
            aiGetMaterialFloat(material, AI_MATKEY_METALLIC_FACTOR, &metallic);
            engineMaterial->metallic = metallic;

            float roughness = 0.5f;
            aiGetMaterialFloat(material, AI_MATKEY_ROUGHNESS_FACTOR, &roughness);
            engineMaterial->roughness = roughness;

            engineMaterial->ambientOcclusion = 1.0f;

            modelData.materials[materialId] = engineMaterial;
            LOG_INFO("Processed Material: %s", materialId.c_str());
            return engineMaterial;
        }
    }

    ModelData Load(const std::string& path) {
        ModelData modelData;
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            LOG_ERROR("ASSIMP Error loading model '%s': %s", path.c_str(), importer.GetErrorString());
            return modelData;
        }

        modelData.rootObject = detail::ProcessNode(scene->mRootNode, scene, modelData, path);

        std::filesystem::path fs_path(path);
        modelData.rootObject->setName(fs_path.stem().string());

        return modelData;
    }
}