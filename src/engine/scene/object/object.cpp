#include "object.h"

object::object(const std::string& name)
{
	id = nextID++;
	transform = addComponent<transformComponent>();
}

object::~object()
{

}

void object::update(float deltaTime)
{
	for (const auto& component : components) {
		component->update(deltaTime);
	}

	//for (const auto& component : children) {
	//	component->update(deltaTime);
	//}
}

const std::string& object::getName() const
{
	return name;
}

int object::getID() const
{
	return id;
}
