#include "object.h"

int Object::nextID;

Object::Object(const std::string& name)
{
	id = nextID++;
	transform = addComponent<transformComponent>();

	canUpdate = true;
}

Object::~Object()
{

}

void Object::update(float deltaTime)
{
	for (const auto& component : components) {
		if (component->canUpdate) {
			component->update(deltaTime);
		}
	}

	//for (const auto& component : children) {
	//	component->update(deltaTime);
	//}
}

const std::string& Object::getName() const
{
	return name;
}

int Object::getID() const
{
	return id;
}
