#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

struct RenderCameraState {
    glm::mat4 view;
    glm::mat4 proj;
    uint32_t viewportWidth;
    uint32_t viewportHeight;
};

class Camera {
public:
    Camera(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f)
        : position(startPosition), worldUp(up), yaw(yaw), pitch(pitch),
        movementSpeed(2.5f), mouseSensitivity(0.1f), fov(60.0f)
    {
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    void processKeyboard(CameraMovement direction, float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        if (direction == CameraMovement::FORWARD)
            position += front * velocity;
        if (direction == CameraMovement::BACKWARD)
            position -= front * velocity;
        if (direction == CameraMovement::LEFT)
            position -= right * velocity;
        if (direction == CameraMovement::RIGHT)
            position += right * velocity;
    }

    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        if (std::abs(xoffset) < 0.0001f && std::abs(yoffset) < 0.0001f)
            return;

        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }
        updateCameraVectors();
    }

    void setSpeed(float value) {
        if (value < 0.0f)
            movementSpeed = 0.0f;
        else if (value > 20.0f)
            movementSpeed = 20.0f;
        else
            movementSpeed = value;
    }

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;

    float fov;

private:
    void updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
};

#endif // SCENE_CAMERA_H
