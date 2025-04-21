#include "ui.h"

#include <imgui.h>
#include <vulkan/imgui_impl_glfw.h>
#include <vulkan/imgui_impl_vulkan.h>

#include <window/window.h>
#include <renderer.h>
#include <vulkan/pipeline.h>

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

	void debug::drawDebugMenu(pipeline& _pipeline)
	{
        static int selectedMesh = -1;
        static bool showPrimitivePopup = true;
        static int selectedPrimitive = -1;

        ImGui::Begin("Debug Layout");

        if (ImGui::Button("Select Primitive")) {
            ImGui::OpenPopup("Primitive Popup");
        }

        if (ImGui::BeginPopup("Primitive Popup")) {
            if (ImGui::Selectable("Cube")) {
                auto [vertices, indices] = primitives::createCube();
                mesh newMesh(vertices, indices);
                size_t vertexByteOffset = _pipeline.getVertexBuffer()->appendVertices(vertices);
                newMesh.vertexOffset = static_cast<uint32_t>(vertexByteOffset / sizeof(vertex));
                size_t indexByteOffset = _pipeline.getIndexBuffer()->appendIndices(indices);
                newMesh.indexOffset = static_cast<uint32_t>(indexByteOffset / sizeof(uint32_t));
                newMesh.indexCount = static_cast<uint32_t>(indices.size());
                newMesh.transform.translation = glm::vec3(0.0f, 0.0f, -5.0f);
                _pipeline.meshes.push_back(newMesh);
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Selectable("Pyramid")) {
                auto [vertices, indices] = primitives::createPyramid();
                mesh newMesh(vertices, indices);
                size_t vertexByteOffset = _pipeline.getVertexBuffer()->appendVertices(vertices);
                newMesh.vertexOffset = static_cast<uint32_t>(vertexByteOffset / sizeof(vertex));
                size_t indexByteOffset = _pipeline.getIndexBuffer()->appendIndices(indices);
                newMesh.indexOffset = static_cast<uint32_t>(indexByteOffset / sizeof(uint32_t));
                newMesh.indexCount = static_cast<uint32_t>(indices.size());
                newMesh.transform.translation = glm::vec3(0.0f, 0.0f, -5.0f);
                _pipeline.meshes.push_back(newMesh);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::Text("Number of Meshes: %zu", _pipeline.meshes.size());
        ImGui::Text("Selected Mesh: %d", selectedMesh);

        ImGui::Separator();
        ImGui::Text("Meshes in Scene:");
        static int selectedMeshLocal = -1;
        for (size_t i = 0; i < _pipeline.meshes.size(); ++i) {
            char label[32];
            sprintf(label, "Mesh %zu", i);
            if (ImGui::Selectable(label, selectedMeshLocal == static_cast<int>(i))) {
                selectedMeshLocal = static_cast<int>(i);
                selectedMesh = selectedMeshLocal;
            }
        }

        ImGui::Separator();
        ImGui::Text("Environment Settings");
        static float sunDir[3] = { 1.0f, 1.0f, -1.0f };

        ImGui::End();
        
        if (selectedMesh >= 0 && selectedMesh < static_cast<int>(_pipeline.meshes.size())) {
            ImGui::SetNextWindowPos(ImVec2(400, 100), ImGuiCond_FirstUseEver);
            ImGui::Begin("Mesh Settings");

            mesh& selected = _pipeline.meshes[selectedMesh];
            ImGui::Text("Edit Mesh %d Transform", selectedMesh);
            float pos[3] = { selected.transform.translation.x, selected.transform.translation.y, selected.transform.translation.z };
            if (ImGui::DragFloat3("Position", pos, 0.1f)) {
                selected.transform.translation = glm::vec3(pos[0], pos[1], pos[2]);
            }
            float rot[3] = { selected.transform.rotation.x, selected.transform.rotation.y, selected.transform.rotation.z };
            if (ImGui::DragFloat3("Rotation", rot, 0.5f)) {
                selected.transform.rotation = glm::vec3(rot[0], rot[1], rot[2]);
            }
            float scale[3] = { selected.transform.scale.x, selected.transform.scale.y, selected.transform.scale.z };
            if (ImGui::DragFloat3("Scale", scale, 0.1f)) {
                selected.transform.scale = glm::vec3(scale[0], scale[1], scale[2]);
            }

            ImGui::Separator();
            ImGui::Text("Material Settings");

            // Diffuse Color
            {
                float diffuse[3] = { selected.material.diffuseColor.r,
                                     selected.material.diffuseColor.g,
                                     selected.material.diffuseColor.b };
                if (ImGui::ColorEdit3("Diffuse Color", diffuse)) {
                    selected.material.diffuseColor = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
                }
            }

            {
                if (ImGui::DragFloat("Ambient Factor", &selected.material.ambientFactor, 0.01f, 0.0f, 5.0f)) {
                    // already
                }
            }


            if (ImGui::Button("Reset Material")) {
                selected.material.diffuseColor = glm::vec3(1.0f);
            }


            ImGui::End();
        }
	}
}