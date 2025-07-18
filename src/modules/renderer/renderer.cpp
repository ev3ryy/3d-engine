#include "renderer.h"

#include "window/window.h"
#include "vulkan/pipeline.h"

#include "world/world.h"
#include "../resources/manager/resource_manager.h"
#include "object/components/transform_component.h"
#include "object/components/mesh_renderer_component.h"
#include "object/components/rigidbody_component.h"

#include "../physics/utils/motion_state.h"
#include "../physics/utils/drawer.h"
#include "../physics/world/physics_world.h"

#include <btBulletDynamicsCommon.h>

#include "camera/camera.h"

#include <GLFW/glfw3.h>
#include <logs.h>

renderer::renderer()
{
	init();
}

renderer::~renderer()
{
	delete _pipeline;
	delete _window;

	LOG_INFO("Renderer module: shutdown");
}

void renderer::init() {
	LOG_INFO("Renderer module: initialized");

	_window = new window(1920, 1080, "Engine");
	_pipeline = new pipeline();
}

void renderer::collectRenderableObjectsRecursive(Object* currentObject, std::vector<RenderObject>& renderObjects, ResourceManager& resourceManager, pipeline* pipeline, float interpolationAlpha) {
	if (!currentObject) {
		return;
	}

	MeshRendererComponent* meshRenderer = currentObject->getComponent<MeshRendererComponent>();
	if (meshRenderer) {
		std::shared_ptr<Mesh> meshPtr = meshRenderer->getMesh();
		const std::string& materialID = meshRenderer->getMaterialID();
		std::shared_ptr<Material> materialPtr = resourceManager.GetMaterial(materialID);

		if (meshPtr && materialPtr) {
			RenderObject renderObj;
			renderObj.mesh = meshPtr.get();


			RigidBodyComponent* rb = currentObject->getComponent<RigidBodyComponent>();
			if (rb && rb->GetBtRigidBody() && rb->GetBtRigidBody()->getMotionState()) {
				glm::vec3 prevPos = rb->getPreviousPhysicsPosition();
				glm::quat prevRot = rb->getPreviousPhysicsRotation();
				glm::vec3 currentPos = rb->getCurrentPhysicsPosition();
				glm::quat currentRot = rb->getCurrentPhysicsRotation();

				glm::vec3 interpolatedPos = glm::mix(prevPos, currentPos, interpolationAlpha);
				glm::quat interpolatedRot = glm::slerp(prevRot, currentRot, interpolationAlpha);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, interpolatedPos);
				model = model * glm::toMat4(interpolatedRot);

				model = model * glm::scale(glm::mat4(1.0f), currentObject->getComponent<TransformComponent>()->getScale());
				renderObj.modelMatrix = model;
			}
			else {
				renderObj.modelMatrix = currentObject->getWorldMatrix();
			}

			MaterialInstance* materialInstance = pipeline->getOrCreateMaterialInstance(*materialPtr);
			renderObj.material = materialInstance;

			if (materialPtr->isDirty) {
				MaterialData data = materialPtr->toMaterialData();
				materialInstance->buffer->writeToBuffer(&data, sizeof(MaterialData));
				materialPtr->isDirty = false;
			}
			renderObjects.push_back(renderObj);
		}
	}

	for (const auto& childPtr : currentObject->getChildren()) {
		collectRenderableObjectsRecursive(childPtr.get(), renderObjects, resourceManager, pipeline, interpolationAlpha);
	}
}

void renderer::syncWithWorld(const World& world, ResourceManager& resourceManager, float interpolationAlpha)
{
	_renderObjects.clear();

	const auto& rootObjects = world.getAllObjects();
	for (const auto& objPtr : rootObjects) {
		collectRenderableObjectsRecursive(objPtr.get(), _renderObjects, resourceManager, _pipeline, interpolationAlpha);
	}
}

void renderer::render(const World& world, ResourceManager& resourceManager, float alpha, PhysicsWorld* physicsWorld)
{
	if (!_pipeline || _pipeline->getDevice() == VK_NULL_HANDLE) {
		LOG_ERROR("Renderer pipeline is not initialized!");
		return;
	}

	VkDevice device = _pipeline->getDevice();
	uint32_t currentFrameIndex = _pipeline->getCurrentFrame();

	vkWaitForFences(device, 1, &_pipeline->inFlightFences[currentFrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(
		device,
		_pipeline->getSwapchain()->swapChain,
		std::numeric_limits<uint64_t>::max(),
		_pipeline->imageAvailableSemaphores[currentFrameIndex],
		VK_NULL_HANDLE,
		&imageIndex
	);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		window::framebufferResized = false;
		_pipeline->getSwapchain()->recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		LOG_CRITICAL("Failed to acquire swap chain image");
	}

	vkResetFences(device, 1, &_pipeline->inFlightFences[currentFrameIndex]);
	vkResetCommandBuffer(_pipeline->commandBuffers[currentFrameIndex], 0);

	RenderFrameData renderData{};

	const Camera& activeCamera = world.getActiveRenderCamera();
	glm::mat4 viewMatrix = activeCamera.getViewMatrix();
	glm::mat4 projMatrix = glm::perspective(
		glm::radians(activeCamera.fov),
		(float)_pipeline->getSwapchain()->swapChainExtent.width / (float)_pipeline->getSwapchain()->swapChainExtent.height,
		activeCamera.getNearPlane(), activeCamera.getFarPlane()
	);

	projMatrix[1][1] *= -1;

	renderData.viewMatrix = viewMatrix;
	renderData.projMatrix = projMatrix;

	_pipeline->updateUniformBuffer(currentFrameIndex, viewMatrix, projMatrix, world.getActiveRenderCamera().position);
	renderData.globalDescriptorSet = _pipeline->getDescriptorSets()[currentFrameIndex];

	ImVec4 clearColor = ImVec4(0.23f, 0.22f, 0.22f, 1.00f);

	VkExtent2D swapchainExtent = _pipeline->getSwapchain()->swapChainExtent;
	renderData.viewportWidth = swapchainExtent.width;
	renderData.viewportHeight = swapchainExtent.height;
	renderData.clearColor = clearColor;
	renderData.imguiDrawData = ImGui::GetDrawData();

	syncWithWorld(world, resourceManager, alpha);

	physicsWorld->debugDrawAllEnabledColliders(world.getAllRawObjects());

	const auto& wireframeVertices = physicsWorld->getDebugDrawer()->getVertices();
	const auto& wireframeIndices = physicsWorld->getDebugDrawer()->getIndices();

	_pipeline->createWireframeBuffers(wireframeVertices, wireframeIndices);
	_pipeline->recordCommandBuffer(_pipeline->commandBuffers[currentFrameIndex], imageIndex, renderData, _renderObjects);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { _pipeline->imageAvailableSemaphores[currentFrameIndex] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_pipeline->commandBuffers[currentFrameIndex];

	VkSemaphore signalSemaphores[] = { _pipeline->renderFinishedSemaphores[currentFrameIndex] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VkQueue graphicsQueue = _pipeline->getGraphicsQueue();
	VkQueue presentQueue = _pipeline->getPresentQueue();

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, _pipeline->inFlightFences[currentFrameIndex]) != VK_SUCCESS) {
		LOG_CRITICAL("Failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapchains[] = { _pipeline->getSwapchain()->swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	bool framebufferResized = window::framebufferResized;
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		window::framebufferResized = false;
		_pipeline->getSwapchain()->recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		LOG_CRITICAL("Failed to present swap chain image");
	}

	_pipeline->setCurrentFrame((currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT);
}

void renderer::waitDeviceIdle() const
{
	if (_pipeline && _pipeline->getDevice() != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(_pipeline->getDevice());
	}
}
