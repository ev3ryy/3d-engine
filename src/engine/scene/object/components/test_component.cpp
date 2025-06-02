#include "test_component.h"

#include <logger.h>

testComponent::testComponent()
{
	canUpdate = false;
	LOG_INFO("Component initialized!");
}

void testComponent::update(float deltaTime)
{
	LOG_INFO("test component updated");
}
