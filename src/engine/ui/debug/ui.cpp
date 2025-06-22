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

    void debug::drawDebugMenu(World& world, renderer& renderer)
    {
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

        if (selectedObject) {
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
    }
}