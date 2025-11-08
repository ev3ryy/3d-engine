#include "renderer.h"

#include <fabric.h>

#include "window/window.h"
//#include "vulkan/pipeline.h"

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
}

renderer::~renderer()
{
	shutdown();
}

void renderer::init() {
	shaderManager = std::make_unique<ShaderManager>();

	pipeline = RendererFabric::createPipeline(API_TYPE::Vulkan, shaderManager.get());

	_window = new window(1920, 1080, "Engine");

	pipeline->init();

	LOG_INFO("Renderer module: initialized");
}

void renderer::shutdown() {
	pipeline->cleanup();
	delete _window;
	
	delete pipeline;

	LOG_INFO("Renderer module: shutdown");
}

void renderer::collectRenderableObjectsRecursive(Object* currentObject, std::vector<RenderObject>& renderObjects, ResourceManager& resourceManager, float interpolationAlpha) {
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
		collectRenderableObjectsRecursive(childPtr.get(), renderObjects, resourceManager, interpolationAlpha);
	}
}

void renderer::syncWithWorld(const World& world, ResourceManager& resourceManager, float interpolationAlpha)
{
	_renderObjects.clear();

	const auto& rootObjects = world.getAllObjects();
	for (const auto& objPtr : rootObjects) {
		collectRenderableObjectsRecursive(objPtr.get(), _renderObjects, resourceManager, interpolationAlpha);
	}
}

void renderer::uploadMesh(Mesh* mesh) {
	if (pipeline) {
		pipeline->uploadMesh(mesh);
	}
}

void renderer::render(const World& world, ResourceManager& resourceManager, float alpha, PhysicsWorld* physicsWorld)
{
	if (!pipeline || !pipeline->IsValid() || !pipeline->getDevice()->IsValid()) {
		LOG_ERROR("Renderer pipeline is not initialized!");
		return;
	}

	FrameRenderStatus status = pipeline->beginFrame();

	if (status == FrameRenderStatus::SwapChainNeedsResize) {
		if (window::framebufferResized) {
			window::framebufferResized = false;
			pipeline->notifyWindowResized();
		}
		return;
	}
	else if (status == FrameRenderStatus::Error) {
		LOG_ERROR("Failed to begin frame.");
		return;
	}

	syncWithWorld(world, resourceManager, alpha);
	physicsWorld->debugDrawAllEnabledColliders(world.getAllRawObjects());

	RenderFrameData frameData(
		_renderObjects,
		physicsWorld->getDebugDrawer()->getVertices(),
		physicsWorld->getDebugDrawer()->getIndices()
	);

	const Camera& activeCamera = world.getActiveRenderCamera();
	frameData.viewMatrix = activeCamera.getViewMatrix();
	frameData.cameraFov = activeCamera.fov;
	frameData.cameraNearPlane = activeCamera.getNearPlane();
	frameData.cameraFarPlane = activeCamera.getFarPlane();
	frameData.cameraPosition = activeCamera.position;

	frameData.sunDirection = world.getSunDirection();
	frameData.sunIntensity = world.getSunIntensity();

	frameData.imguiDrawData = ImGui::GetDrawData();
	frameData.clearColor = ImVec4(0.23f, 0.22f, 0.22f, 1.00f);

	pipeline->drawFrame(frameData);

	status = pipeline->endFrame();

	if (status == FrameRenderStatus::SwapChainNeedsResize || window::framebufferResized) {
		window::framebufferResized = false;
		pipeline->notifyWindowResized();
	}
	else if (status == FrameRenderStatus::Error) {
		LOG_CRITICAL("Failed to present swap chain image");
	}
}

void renderer::waitDeviceIdle() const
{
	if (pipeline && pipeline->getDevice()->IsValid()) {
		pipeline->getDevice()->waitIdle();
	}
}
