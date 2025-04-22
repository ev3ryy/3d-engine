#include "component.h"

void component::setOwner(object* obj)
{
	owner = obj;
}

object* component::getOwner() const
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
