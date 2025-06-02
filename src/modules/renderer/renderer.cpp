#include "renderer.h"

#include "window/window.h"
#include "vulkan/pipeline.h"

#include "world/world.h"
#include "object/components/transform_component.h"
#include "object/components/mesh_renderer_component.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

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

void renderer::render(const World& world)
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
		_pipeline->getSwapchain()->recreateSwapChain(_pipeline->getRenderPass(), _pipeline->getDepthImageView());
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
		0.1f, 1000.0f // near/far
	);

	projMatrix[1][1] *= -1;

	renderData.viewMatrix = viewMatrix;
	renderData.projMatrix = projMatrix;

	_pipeline->updateUniformBuffer(currentFrameIndex, viewMatrix, projMatrix);
	renderData.globalDescriptorSet = _pipeline->getDescriptorSets()[currentFrameIndex];

	std::vector<Object*> renderableObjects = world.getRenderableObjects();

	for (Object* obj : renderableObjects) {
		transformComponent* transform = obj->getComponent<transformComponent>();
		MeshRendererComponent* meshRenderer = obj->getComponent<MeshRendererComponent>();

		if (!transform || !meshRenderer) {
			continue;
		}

		std::shared_ptr<Mesh> meshPtr = meshRenderer->getMesh();
		std::shared_ptr<Material> materialPtr = meshRenderer->getMaterial();

		if (!meshPtr || !materialPtr) {
			continue;
		}

		Mesh* mesh = meshPtr.get();
		Material* material = materialPtr.get();

		RenderItem renderItem{};
		renderItem.modelMatrix = transform->getWorldMatrix();
		//renderItem.material = material;
		renderItem.indexCount = mesh->indexCount;
		renderItem.indexOffset = mesh->indexOffset;
		renderItem.vertexOffset = mesh->vertexCount;

		renderData.renderItems.push_back(renderItem);
	}

	ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

	VkExtent2D swapchainExtent = _pipeline->getSwapchain()->swapChainExtent;
	renderData.viewportWidth = swapchainExtent.width;
	renderData.viewportHeight = swapchainExtent.height;
	renderData.clearColor = clearColor;
	renderData.imguiDrawData = ImGui::GetDrawData();

	_pipeline->recordCommandBuffer(_pipeline->commandBuffers[currentFrameIndex], imageIndex, renderData);

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
		_pipeline->getSwapchain()->recreateSwapChain(_pipeline->getRenderPass(), _pipeline->getDepthImageView());
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
