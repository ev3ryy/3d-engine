#include "world.h"

#include "../object/components/rigidbody_component.h"

#include <logs.h>

World::World(Physics* physicsFacade) : m_physicsFacade(physicsFacade)
{
}

void World::addObject(std::unique_ptr<Object> obj)
{
	if (!obj) {
		return;
	}

	if (obj->getParent() != nullptr) {
		return;
	}

	addObjectsToMapsRecursive(obj.get());
	obj->setWorld(this);
	objects.push_back(std::move(obj));
}

void World::removeRootObject(Object* obj)
{
	if (!obj) return;

	removeObjectsFromMapsRecursive(obj);

	objects.erase(std::remove_if(objects.begin(), objects.end(),
		[obj](const std::unique_ptr<Object>& ptr) { return ptr.get() == obj; }),
		objects.end());
}

const std::vector<std::unique_ptr<Object>>& World::getAllObjects() const
{
	return objects;
}

void World::addObjectsToMapsRecursive(Object* obj)
{
	if (!obj) return;
	objectIdMap[obj->getID()] = obj;
	objectNameMap[obj->getName()] = obj;

	for (const auto& child : obj->getChildren()) {
		addObjectsToMapsRecursive(child.get());
	}
}

void World::removeObjectsFromMapsRecursive(Object* obj)
{
	if (!obj) return;
	objectIdMap.erase(obj->getID());
	objectNameMap.erase(obj->getName());
	for (const auto& child : obj->getChildren()) {
		removeObjectsFromMapsRecursive(child.get());
	}
}

Object* World::findObjectByID(int id)
{
	auto it = objectIdMap.find(id);
	if (it != objectIdMap.end()) {
		return it->second;
	}

	return nullptr;
}

Object* World::findObjectByName(const std::string& name)
{
	auto it = objectNameMap.find(name);
	if (it != objectNameMap.end()) {
		return it->second;
	}

	return nullptr;
}

const Camera& World::getActiveRenderCamera() const
{
	if (!activeRenderCamera) {
		LOG_ERROR("No active render camera is set in the world");
		return Camera();
	}

	return *activeRenderCamera;
}

void World::setActiveRenderCamera(const Camera* cam)
{
	if (!cam) {
		LOG_ERROR("setActiveRenderCamera doesn`t have cam pointer");
		return;
	}
	
	activeRenderCamera = cam;
}

//std::vector<Object*> World::getRenderableObjects() const
//{
//	std::vector<Object*> renderables;
//	for (const auto& obj : objects) {
//		if (obj->getComponent<MeshRendererComponent>()) {
//			renderables.push_back(obj.get());
//		}
//	}
//
//	return renderables;
//}

void World::update(float deltaTime)
{
	for (const auto& obj_ptr : objects) {
		if (obj_ptr) {
			obj_ptr->update(deltaTime);
		}
	}

	if (m_physicsFacade) {
		// synchronizeTransformsToPhysics();

		m_physicsFacade->update(deltaTime);

		synchronizeTransformsFromPhysics();
	}
}

void World::synchronizeTransformsFromPhysics() {
	for (const auto& obj : objects) {

		std::function<void(Object*)> syncFunc =
			[&](Object* currentObj) {
			if (auto* rb = currentObj->getComponent<RigidBodyComponent>()) {
				if (auto* transform = currentObj->getTransform()) {

					transform->position = m_physicsFacade->getPosition(rb->getBodyHandle());
					// transform->rotation = m_physicsFacade->getRotation(rb->getBodyHandle());
				}
			}
			for (const auto& child : currentObj->getChildren()) {
				syncFunc(child.get());
			}
			};
		syncFunc(obj.get());
	}
}

void World::updateObjectRecursive(Object* obj, float deltaTime) {
	if (!obj) return;

	for (const auto& child : obj->getChildren()) {
		updateObjectRecursive(child.get(), deltaTime);
	}
}

void World::clear()
{
	objects.clear();
	objectIdMap.clear();
	objectNameMap.clear();
	activeRenderCamera = nullptr;
	Object::nextID = 0;
}
