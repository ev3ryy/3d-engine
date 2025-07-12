#include "camera_component.h"

#include "transform_component.h"
#include "../object.h"

#include <window/window.h>
#include <input.h>

#include <logs.h>

void CameraComponent::update(float deltaTime) {
    TransformComponent* transform = getOwner()->getComponent<TransformComponent>();
    if (!transform) return;

    camera.position = transform->position;

    bool cameraControlActive = (glfwGetMouseButton(window::_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

    if (cameraControlActive) {
        glfwSetInputMode(window::_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        float xoffset, yoffset;
        Input::instance().getMouseDelta(xoffset, yoffset);
        if (xoffset != 0.0f || yoffset != 0.0f) {
            camera.processMouseMovement(xoffset, yoffset);
        }

        bool shiftDown = Input::instance().isKeyDown(keycode::LShift);
        float speedMultiplier = shiftDown ? 2.5f : 1.0f;
        float adjustedDeltaTime = deltaTime * speedMultiplier;

        if (Input::instance().isKeyDown(keycode::W))
            camera.processKeyboard(CameraMovement::FORWARD, adjustedDeltaTime);
        if (Input::instance().isKeyDown(keycode::S))
            camera.processKeyboard(CameraMovement::BACKWARD, adjustedDeltaTime);
        if (Input::instance().isKeyDown(keycode::A))
            camera.processKeyboard(CameraMovement::LEFT, adjustedDeltaTime);
        if (Input::instance().isKeyDown(keycode::D))
            camera.processKeyboard(CameraMovement::RIGHT, adjustedDeltaTime);

        float scrollX, scrollY;
        Input::instance().getScrollDelta(scrollX, scrollY);
        if (scrollY != 0.0f) {
            const float speedSensitivity = 0.2f;
            camera.setSpeed(camera.movementSpeed + scrollY * speedSensitivity);
        }

    }
    else {
        glfwSetInputMode(window::_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        float dummyX, dummyY;
        Input::instance().getMouseDelta(dummyX, dummyY);
    }

    transform->position = camera.position;
}