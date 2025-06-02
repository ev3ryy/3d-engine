#include "test_object.h"
#include "components/test_component.h"

#include <logger.h>

testObject::testObject(const std::string& name) : Object(name)
{
	simpleComponent = addComponent<testComponent>();

	LOG_INFO("testObject '%s' initialized!", name.c_str());

	canUpdate = false;
}

testObject::~testObject()
{
}

void testObject::update(float deltaTime)
{
	Object::update(deltaTime);

	LOG_INFO("Object updated");
}
