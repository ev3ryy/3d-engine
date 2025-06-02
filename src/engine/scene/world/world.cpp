#include "world.h"

#include "../object/test_object.h"
#include "../object/components/test_component.h"

World::World()
{
	testObject* obj = createObject<testObject>();
}

void World::addObject(std::unique_ptr<Object> obj)
{
	objects.push_back(std::move(obj));
}

const std::vector<std::unique_ptr<Object>>& World::getAllObjects() const
{
	return objects;
}

Object* World::findObjectByID(int id)
{
	auto it = objectIdMap.find(id);
	if (it != objectIdMap.end()) {
		return it->second;
	}

	return nullptr;
}

Object* World::findObjectByName(std::string& name)
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
}

std::vector<Object*> World::getRenderableObjects() const
{
	std::vector<Object*> renderables;
	for (const auto& obj : objects) {
		if (obj->getComponent<MeshRendererComponent>()) {
			renderables.push_back(obj.get());
		}
	}

	return renderables;
}

void World::update(float deltaTime)
{
	for (const auto& obj : objects) {
		if (obj->canUpdate) {
			obj->update(deltaTime);
		}
	}
}

void World::clear()
{
	objects.clear();
	Object::nextID = 0;
}
