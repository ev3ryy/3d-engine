#include "ui.h"

#include <imgui.h>
#include <vulkan/imgui_impl_glfw.h>
#include <vulkan/imgui_impl_vulkan.h>

#include <window/window.h>
#include <renderer.h>
#include <vulkan/pipeline.h>

#include "object/component.h"
#include "object/components/transform_component.h"
#include "object/components/mesh_renderer_component.h"
#include "object/components/camera_component.h"

#include "mesh/mesh.h"
#include "material/material.h"

#include "object/object.h"
#include "world/world.h"

#include "../../resources/manager/resource_manager.h"

#include <mesh/primitives/primitives.h>

#include <spdlog/spdlog.h>

namespace ui {
	static void check_vk_result(VkResult err)
	{
		if (err == VK_SUCCESS)
			return;

		LOG_ERROR("VkResult = %d", err);
		if (err < 0)
			abort();
	}

	void debug::initialize(renderer& _renderer) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForVulkan(window::_window, false);
		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = _renderer.getPipeline()->getInstance();
		initInfo.PhysicalDevice = _renderer.getPipeline()->getPhysicalDevice();
		initInfo.Device = _renderer.getPipeline()->getDevice();
		initInfo.QueueFamily = _renderer.getPipeline()->getQueueFamily();
		initInfo.Queue = _renderer.getPipeline()->getGraphicsQueue();
		initInfo.PipelineCache = VK_NULL_HANDLE;
		initInfo.DescriptorPool = _renderer.getPipeline()->getDescriptorPool();
		initInfo.RenderPass = _renderer.getPipeline()->getRenderPass();
		initInfo.Subpass = 0;
		initInfo.MinImageCount = _renderer.getPipeline()->getMinImageCount();
		initInfo.ImageCount = _renderer.getPipeline()->getImageCount();
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.Allocator = nullptr;
		initInfo.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&initInfo);
	}

    Object* drawSceneHierarchy(World& world, renderer& renderer) {
        static Object* selectedObject = nullptr;
        ImGui::Begin("Scene Hierarchy");

        if (ImGui::Button("Create Primitive")) {
            ImGui::OpenPopup("Primitive Popup");
        }

        if (ImGui::BeginPopup("Primitive Popup")) {
            auto createAndSpawnPrimitive = [&](const std::string& primitiveName, const std::function<std::pair<std::vector<vertex>, std::vector<uint32_t>>()>& generator) {
                if (!ResourceManager::Get().GetMesh(primitiveName)) {
                    ResourceManager::Get().CreatePrimitive(primitiveName, generator);
                }

                std::shared_ptr<Material> defaultMat = ResourceManager::Get().GetMaterial("DefaultPBRMaterial");
                if (!defaultMat) {
                    defaultMat = std::make_shared<Material>();
                    defaultMat->name = "DefaultPBRMaterial";
                    defaultMat->albedoColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
                    defaultMat->roughness = 0.5f;
                    defaultMat->metallic = 0.0f;
                    defaultMat->ambientOcclusion = 1.0f;
                    ResourceManager::Get().RegisterMaterial("DefaultPBRMaterial", defaultMat);
                }

                auto mesh = ResourceManager::Get().GetMesh(primitiveName);

                if (!mesh || !defaultMat) {
                    LOG_ERROR("Failed to get mesh (%s) or default material (DefaultPBRMaterial) from ResourceManager!", primitiveName.c_str());
                    return;
                }

                auto newObject = world.createObject(primitiveName);
                newObject->addComponent<transformComponent>();
                newObject->addComponent<MeshRendererComponent>(mesh, defaultMat->name);

                ImGui::CloseCurrentPopup();
                };

            if (ImGui::Selectable("Cube")) { createAndSpawnPrimitive("Cube", primitives::createCube); }
            if (ImGui::Selectable("Pyramid")) { createAndSpawnPrimitive("Pyramid", primitives::createPyramid); }

            ImGui::EndPopup();
        }

        ImGui::Separator();
        ImGui::Text("Objects in Scene:");

        const auto& allObjects = world.getAllObjects();
        for (const auto& objPtr : allObjects) {
            Object* currentObject = objPtr.get();

            if (currentObject->getComponent<CameraComponent>() != nullptr) {
                continue;
            }

            char label[128];
            snprintf(label, sizeof(label), "%s (ID: %u)", currentObject->getName().c_str(), currentObject->getID());

            if (ImGui::Selectable(label, selectedObject == currentObject)) {
                selectedObject = currentObject;
            }
        }

        ImGui::End();
        return selectedObject;
    }


    void drawInspector(Object* selectedObject) {
        if (!selectedObject) return;

        ImGui::Begin("Inspector");

        ImGui::Text("Editing: %s", selectedObject->getName().c_str());
        ImGui::Separator();

        if (transformComponent* transform = selectedObject->getComponent<transformComponent>()) {
            ImGui::Text("Transform");
            ImGui::DragFloat3("Position", &transform->position.x, 0.1f);
            ImGui::DragFloat3("Rotation", &transform->rotation.x, 0.5f);
            ImGui::DragFloat3("Scale", &transform->scale.x, 0.1f);
        }
        else {
            ImGui::Text("Object has no Transform Component.");
        }

        ImGui::Separator();

        if (MeshRendererComponent* meshRenderer = selectedObject->getComponent<MeshRendererComponent>()) {
            std::shared_ptr<Material> materialDef = ResourceManager::Get().GetMaterial(meshRenderer->getMaterialID());

            if (materialDef) {
                ImGui::Text("Material: %s", materialDef->name.c_str());

                if (ImGui::ColorEdit4("Albedo Color", &materialDef->albedoColor.r)) {
                    materialDef->isDirty = true;
                }
                if (ImGui::DragFloat("Roughness", &materialDef->roughness, 0.01f, 0.0f, 1.0f)) {
                    materialDef->isDirty = true;
                }
                if (ImGui::DragFloat("Metallic", &materialDef->metallic, 0.01f, 0.0f, 1.0f)) {
                    materialDef->isDirty = true;
                }
                if (ImGui::DragFloat("Ambient Occlusion", &materialDef->ambientOcclusion, 0.01f, 0.0f, 1.0f)) {
                    materialDef->isDirty = true;
                }

                if (ImGui::BeginCombo("Select Material", materialDef->name.c_str())) {
                    for (const auto& [id, mat] : ResourceManager::Get().GetAllMaterials()) {
                        bool is_selected = (id == materialDef->name);
                        if (ImGui::Selectable(id.c_str(), is_selected)) {
                            meshRenderer->setMaterialId(id);
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
            else {
                ImGui::Text("Material not found: %s", meshRenderer->getMaterialID().c_str());
            }
        }
        else {
            ImGui::Text("Object has no Mesh Renderer Component.");
        }

        ImGui::End();
    }

    void drawAssetBrowser(World& world, renderer& renderer) {
        static char modelPathBuffer[256] = "assets/models/sponza/sponza.obj";
        static std::string selectedMeshID;

        ImGui::Begin("Asset Browser");

        ImGui::Text("Load New Model");
        ImGui::InputText("##ModelPath", modelPathBuffer, sizeof(modelPathBuffer));
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            ResourceManager::Get().LoadModel(modelPathBuffer);
        }
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Meshes")) {
            for (const auto& [id, mesh] : ResourceManager::Get().GetAllMeshes()) {
                if (ImGui::Selectable(id.c_str(), selectedMeshID == id)) {
                    selectedMeshID = id;
                }
            }
        }

        if (ImGui::CollapsingHeader("Materials")) {
            for (const auto& [id, material] : ResourceManager::Get().GetAllMaterials()) {
                ImGui::Text(id.c_str());
            }
        }

        ImGui::Separator();

        if (!selectedMeshID.empty()) {
            ImGui::Text("Selected: %s", selectedMeshID.c_str());
            if (ImGui::Button("Spawn in World")) {
                std::shared_ptr<Mesh> meshTemplate = ResourceManager::Get().GetMesh(selectedMeshID);
                if (meshTemplate) {
                    if (meshTemplate->vertexCount == 0) {
                        LOG_INFO("First time spawning mesh '%s'. Uploading to GPU...", selectedMeshID.c_str());

                        auto pipeline = renderer.getPipeline();
                        size_t vertexByteOffset = pipeline->getVertexBuffer()->appendVertices(meshTemplate->getVertices());
                        size_t indexByteOffset = pipeline->getIndexBuffer()->appendIndices(meshTemplate->getIndices());

                        meshTemplate->vertexOffset = static_cast<uint32_t>(vertexByteOffset / sizeof(vertex));
                        meshTemplate->indexOffset = static_cast<uint32_t>(indexByteOffset / sizeof(uint32_t));
                        meshTemplate->vertexCount = static_cast<uint32_t>(meshTemplate->getVertices().size());
                        meshTemplate->indexCount = static_cast<uint32_t>(meshTemplate->getIndices().size());
                    }

                    std::string materialToUseID = meshTemplate->materialId_;
                    if (materialToUseID.empty() || !ResourceManager::Get().GetMaterial(materialToUseID)) {
                        std::shared_ptr<Material> defaultMat = ResourceManager::Get().GetMaterial("DefaultPBRMaterial");
                        if (!defaultMat) {
                            defaultMat = std::make_shared<Material>();
                            defaultMat->name = "DefaultPBRMaterial";
                            defaultMat->albedoColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
                            defaultMat->roughness = 0.5f;
                            defaultMat->metallic = 0.0f;
                            defaultMat->ambientOcclusion = 1.0f;
                            ResourceManager::Get().RegisterMaterial("DefaultPBRMaterial", defaultMat);
                        }
                        materialToUseID = defaultMat->name;
                    }

                    if (materialToUseID.empty()) {
                        LOG_ERROR("Could not determine a suitable material ID for spawning mesh '%s'!", selectedMeshID.c_str());
                        return;
                    }   

                    auto newObject = world.createObject(selectedMeshID);
                    newObject->addComponent<transformComponent>();
                    newObject->addComponent<MeshRendererComponent>(meshTemplate, materialToUseID);
                }
            }
        }

        ImGui::End();
    }

    void debug::drawDebugMenu(World& world, renderer& renderer)
    {
        Object* selectedObject = drawSceneHierarchy(world, renderer);
        drawInspector(selectedObject);
        drawAssetBrowser(world, renderer);
    }
}