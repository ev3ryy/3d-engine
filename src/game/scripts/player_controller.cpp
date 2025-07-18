#include "player_controller.h"

#include <logs.h>

#include <scene/object/components/transform_component.h>
#include <scene/object/object.h>

REGISTER_SCRIPT(PlayerController);

PlayerController::PlayerController()
	: ScriptBase("PlayerController")
{
}

void PlayerController::OnCreate()
{
	LOG_INFO("[player controller] was created");

	transform = GetComponent<TransformComponent>();
}

void PlayerController::OnUpdate(float deltaTime)
{
	//LOG_INFO("[player controller] was created");
	if (transform) {
		transform->position = glm::vec3(transform->position.x + 10.0f * deltaTime, transform->position.y, transform->position.z);
	}
}

void PlayerController::OnDestroy()
{
	LOG_INFO("[player controller] was created");
}
