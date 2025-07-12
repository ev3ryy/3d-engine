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
#include "object/components/script_component.h"
#include "object/components/rigidbody_component.h"

#include "mesh/mesh.h"
#include "material/material.h"

#include "object/object.h"
#include "world/world.h"

#include "../../resources/manager/resource_manager.h"

#include <mesh/primitives/primitives.h>

#include <logs.h>

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
		initInfo.RenderPass = _renderer.getPipeline()->getLightingRenderPass();
		initInfo.Subpass = 0;
		initInfo.MinImageCount = _renderer.getPipeline()->getMinImageCount();
		initInfo.ImageCount = _renderer.getPipeline()->getImageCount();
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.Allocator = nullptr;
		initInfo.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&initInfo);
	}

    static void drawObjectNode(Object* object, Object*& selectedObject) {
        const auto& children = object->getChildren();
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (selectedObject == object) {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        }
        if (children.empty()) {
            nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        char label[256];
        snprintf(label, sizeof(label), "%s (ID: %u)", object->getName().c_str(), object->getID());

        bool nodeOpen = ImGui::TreeNodeEx(object, nodeFlags, "%s", label);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            selectedObject = object;
        }

        if (nodeOpen) {
            for (const auto& childPtr : children) {
                drawObjectNode(childPtr.get(), selectedObject);
            }
            if (!children.empty()) {
                ImGui::TreePop();
            }
        }
        else if (children.empty()) {
        }
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
                newObject->addComponent<MeshRendererComponent>(mesh, defaultMat->name);

                ImGui::CloseCurrentPopup();
                };

            if (ImGui::Selectable("Cube")) { createAndSpawnPrimitive("Cube", primitives::createCube); }
            if (ImGui::Selectable("Pyramid")) { createAndSpawnPrimitive("Pyramid", primitives::createPyramid); }

            //if (ImGui::Selectable("Sphere")) { createAndSpawnPrimitive("Sphere", primitives::createSphere); }
            //if (ImGui::Selectable("Plane")) { createAndSpawnPrimitive("Plane", primitives::createPlane); }

            ImGui::EndPopup();
        }

        if (ImGui::Button("Create Empty Object")) {
            world.createObject("Empty Object");
        }
        ImGui::SameLine();
        if (selectedObject && ImGui::Button("Delete Selected")) {
            if (selectedObject->getParent() == nullptr) {
                world.removeRootObject(selectedObject);
            }
            else {
                selectedObject->getParent()->removeChild(selectedObject);
            }
            selectedObject = nullptr;
        }

        ImGui::Separator();
        ImGui::Text("Objects in Scene:");

        const auto& allRootObjects = world.getAllObjects();
        for (const auto& objPtr : allRootObjects) {
            Object* currentObject = objPtr.get();

             if (currentObject->getComponent<CameraComponent>() != nullptr) {
             	continue;
             }

            drawObjectNode(currentObject, selectedObject);
        }

        ImGui::End();
        return selectedObject;
    }

	void drawInspector(Object* selectedObject) {
		if (!selectedObject) return;

		ImGui::Begin("Inspector");

		char nameBuffer[256];
		strncpy(nameBuffer, selectedObject->getName().c_str(), sizeof(nameBuffer) - 1);
		nameBuffer[sizeof(nameBuffer) - 1] = '\0';
		if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
			selectedObject->setName(nameBuffer);
		}

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

        ImGui::Separator();

        if (RigidBodyComponent* rb = selectedObject->getComponent<RigidBodyComponent>()) {
            ImGui::Text("Rigid Body");
            ImGui::DragFloat("Mass", &rb->mass, 0.1f, 0.0f, 1000.0f);
            ImGui::DragFloat("Sphere Radius", &rb->sphereRadius, 0.05f, 0.1f, 100.0f);
            // TODO: ѕри изменении параметров нужно пересоздавать тело в физическом движке
        }

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("AddComponentPopup");
        }

        if (ImGui::BeginPopup("AddComponentPopup")) {
            ImGui::Text("Available Components");
            ImGui::Separator();

            if (ImGui::Selectable("Rigid Body")) {
                if (!selectedObject->getComponent<RigidBodyComponent>()) {
                    selectedObject->addComponent<RigidBodyComponent>();
                    selectedObject->getComponent<RigidBodyComponent>()->isDirty = true;
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Available Scripts");
            ImGui::Separator();

            const auto& scriptFactories = ResourceManager::Get().GetAllScriptFactories();

            if (scriptFactories.empty()) {
                ImGui::TextDisabled("No scripts found in Game.dll");
            }
            else {
                for (const auto& [name, factory] : scriptFactories) {
                    if (ImGui::Selectable(name.c_str())) {
                        auto newScriptInstance = ResourceManager::Get().CreateScriptInstance(name);
                        if (newScriptInstance) {
                            selectedObject->addComponent<ScriptComponent>(std::move(newScriptInstance));
                        }
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
            ImGui::EndPopup();
        }

		ImGui::End();
	}

	void drawAssetBrowser(World& world, renderer& renderer) {
		static char modelPathBuffer[256] = "Chair.fbx";
		static std::string selectedMeshID;

		ImGui::Begin("Asset Browser");

		ImGui::Text("Load New Model");
		ImGui::InputText("##ModelPath", modelPathBuffer, sizeof(modelPathBuffer));
		ImGui::SameLine();
		if (ImGui::Button("Load")) {
			ResourceManager::Get().LoadModel(modelPathBuffer);
		}
		ImGui::Separator();

		static std::string selectedModelID;
		if (ImGui::CollapsingHeader("Models")) {
			for (const auto& [id, prefab] : ResourceManager::Get().GetAllModelPrefabs()) {
				if (ImGui::Selectable(id.c_str(), selectedModelID == id)) {
					selectedModelID = id;
				}
			}
		}

		if (ImGui::CollapsingHeader("Materials")) {
			for (const auto& [id, material] : ResourceManager::Get().GetAllMaterials()) {
				ImGui::Text(id.c_str());
			}
		}

		ImGui::Separator();

        if (!selectedModelID.empty()) {
            ImGui::Text("Selected Model: %s", selectedModelID.c_str());
            if (ImGui::Button("Spawn in World")) {
                Object* prefab = ResourceManager::Get().GetModelPrefab(selectedModelID);
                if (prefab) {
                    std::unique_ptr<Object> instance = prefab->deepCopy();

                    std::vector<Object*> allChildren;
                    std::function<void(Object*)> collectChildren =
                        [&](Object* current) {
                        if (!current) return;
                        allChildren.push_back(current);
                        for (const auto& child : current->getChildren()) {
                            collectChildren(child.get());
                        }
                        };
                    collectChildren(instance.get());

                    for (Object* obj : allChildren) {
                        if (auto* mrc = obj->getComponent<MeshRendererComponent>()) {
                            auto mesh = mrc->getMesh();
                            if (mesh && mesh->vertexCount == 0) {
                                auto pipeline = renderer.getPipeline();
                                size_t vertexByteOffset = pipeline->getVertexBuffer()->appendVertices(mesh->getVertices());
                                size_t indexByteOffset = pipeline->getIndexBuffer()->appendIndices(mesh->getIndices());

                                mesh->vertexOffset = static_cast<uint32_t>(vertexByteOffset / sizeof(vertex));
                                mesh->indexOffset = static_cast<uint32_t>(indexByteOffset / sizeof(uint32_t));
                                mesh->vertexCount = static_cast<uint32_t>(mesh->getVertices().size());
                                mesh->indexCount = static_cast<uint32_t>(mesh->getIndices().size());
                            }
                        }
                    }

                    world.addObject(std::move(instance));
                }
            }
        }

		ImGui::End();
	}

    void drawLightingControls(pipeline& pipelineInstance) {
        ImGui::Begin("Lighting Settings");

        ImGui::Text("Sun Light");

        glm::vec3 currentSunLightDirection = pipelineInstance.sunDirection;
        if (ImGui::SliderFloat3("Direction", &currentSunLightDirection.x, -1.0f, 1.0f)) {
            pipelineInstance.sunDirection = currentSunLightDirection;
        }

        float currentSunLightIntensity = pipelineInstance.sunIntesnity;
        if (ImGui::SliderFloat("Intensity", &currentSunLightIntensity, 0.0f, 200.0f)) {
            pipelineInstance.sunIntesnity = currentSunLightIntensity;
        }

        ImGui::End();
    }

    void debug::drawDebugMenu(World& world, renderer& renderer)
    {
        Object* selectedObject = drawSceneHierarchy(world, renderer);
        drawInspector(selectedObject);
        drawAssetBrowser(world, renderer);
        drawLightingControls(*renderer.getPipeline());
    }
}