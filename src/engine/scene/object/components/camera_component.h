#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "../component.h"
#include "../../camera/camera.h"

class CameraComponent : public component {
public:
    static const bool isUnique = true;

    Camera camera;

    CameraComponent(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f
    ) : camera(position, up, yaw, pitch) {
    }

    ~CameraComponent() override = default;

    void update(float deltaTime) override;
};

#endif // CAMERA_COMPONENT_H