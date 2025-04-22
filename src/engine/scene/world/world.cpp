#include "world.h"

world::world()
{

}

void world::addObject(std::unique_ptr<object> obj)
{
	objects.push_back(std::move(obj));
}

const std::vector<std::unique_ptr<object>>& world::getAllObjects() const
{
	return objects;
}

void world::update(float deltaTime)
{
	for (const auto& obj : objects) {
		obj->update(deltaTime);
	}
}

void world::clear()
{
	objects.clear();
	object::nextID = 0;
}
