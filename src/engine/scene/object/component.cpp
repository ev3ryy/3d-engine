#include "component.h"

component::component()
{
	canUpdate = true;
}

void component::setOwner(Object* obj)
{
	owner = obj;
}

Object* component::getOwner() const
{
	return owner;
}

void component::addedToObject()
{
	// something
}

void component::update(float deltaTime)
{
	// something
}
