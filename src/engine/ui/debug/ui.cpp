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
            auto createPrimitive = [&](const std::string& name, const auto& primitiveGenerator) {
                auto newObject = world.createObject(name);
                auto [vertices, indices] = primitiveGenerator();

                auto pipeline = renderer.getPipeline();
                size_t vertexByteOffset = pipeline->getVertexBuffer()->appendVertices(vertices);
                size_t indexByteOffset = pipeline->getIndexBuffer()->appendIndices(indices);

                auto mesh = std::make_shared<Mesh>(vertices, indices);
                mesh->vertexOffset = static_cast<uint32_t>(vertexByteOffset / sizeof(vertex));
                mesh->indexOffset = static_cast<uint32_t>(indexByteOffset / sizeof(uint32_t));
                mesh->indexCount = static_cast<uint32_t>(indices.size());
                mesh->vertexCount = static_cast<uint32_t>(vertices.size());

                auto material = std::make_shared<Material>();

                newObject->addComponent<MeshRendererComponent>(mesh, material);

                ImGui::CloseCurrentPopup();
                };

            if (ImGui::Selectable("Cube")) {
                createPrimitive("Cube", primitives::createCube);
            }
            if (ImGui::Selectable("Pyramid")) {
                createPrimitive("Pyramid", primitives::createPyramid);
            }

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

            char label[64];
            snprintf(label, sizeof(label), "%s (ID: %d)", currentObject->getName().c_str(), currentObject->getID());

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

        // ... ваш код инспектора остается без изменений ...
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
            if (std::shared_ptr<Material> material = meshRenderer->getMaterial()) {
                ImGui::Text("Material");
                ImGui::ColorEdit3("Diffuse Color", &material->diffuseColor.r);
                ImGui::DragFloat("Ambient Factor", &material->ambientFactor, 0.01f, 0.0f, 5.0f);
            }
            else {
                ImGui::Text("Object has no Material.");
            }
        }
        else {
            ImGui::Text("Object has no Mesh Renderer Component.");
        }

        ImGui::End();
    }

    void drawAssetBrowser(World& world, renderer& renderer) {
        static char modelPathBuffer[256] = "assets/models/sponza/sponza.obj"; // Путь по умолчанию
        static std::string selectedMeshID;

        ImGui::Begin("Asset Browser");

        // --- Секция загрузки ---
        ImGui::Text("Load New Model");
        ImGui::InputText("##ModelPath", modelPathBuffer, sizeof(modelPathBuffer));
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            ResourceManager::Get().LoadModel(modelPathBuffer);
        }
        ImGui::Separator();

        // --- Секция отображения ресурсов ---
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

        // --- Секция спавна объекта ---
        if (!selectedMeshID.empty()) {
            ImGui::Text("Selected: %s", selectedMeshID.c_str());
            if (ImGui::Button("Spawn in World")) {

                // Получаем "шаблон" меша из менеджера
                std::shared_ptr<Mesh> meshTemplate = ResourceManager::Get().GetMesh(selectedMeshID);
                if (meshTemplate) {
                    // Проверяем, был ли этот меш уже загружен на GPU.
                    // Мы используем vertexCount как флаг. Если он 0, значит меш еще только на CPU.
                    if (meshTemplate->vertexCount == 0) {
                        LOG_INFO("First time spawning mesh '%s'. Uploading to GPU...", selectedMeshID.c_str());

                        // ЭТОТ КОД АНАЛОГИЧЕН ВАШЕМУ КОДУ СОЗДАНИЯ ПРИМИТИВОВ
                        auto pipeline = renderer.getPipeline();
                        size_t vertexByteOffset = pipeline->getVertexBuffer()->appendVertices(meshTemplate->getVertices());
                        size_t indexByteOffset = pipeline->getIndexBuffer()->appendIndices(meshTemplate->getIndices());

                        // Обновляем данные прямо в меше, который хранится в ResourceManager.
                        // Теперь все последующие спавны этого меша будут использовать уже готовые оффсеты.
                        meshTemplate->vertexOffset = static_cast<uint32_t>(vertexByteOffset / sizeof(vertex));
                        meshTemplate->indexOffset = static_cast<uint32_t>(indexByteOffset / sizeof(uint32_t));
                        meshTemplate->vertexCount = static_cast<uint32_t>(meshTemplate->getVertices().size());
                        meshTemplate->indexCount = static_cast<uint32_t>(meshTemplate->getIndices().size());
                    }

                    // Получаем материал, используя ID, который мы сохранили в меше
                    std::shared_ptr<Material> material = nullptr;//ResourceManager::Get().GetMaterial(meshTemplate->materialId_);
                    if (!material) {
                        // Если материала нет, создаем и регистрируем материал по умолчанию
                        material = std::make_shared<Material>();
                        material->name = "Default Material";
                        //ResourceManager::Get().RegisterMaterial("default", material);
                    }

                    // Создаем объект в мире
                    auto newObject = world.createObject(selectedMeshID);
                    newObject->addComponent<MeshRendererComponent>(meshTemplate, material);
                    // TransformComponent, скорее всего, добавляется в createObject, если нет - добавьте
                    newObject->addComponent<transformComponent>();
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