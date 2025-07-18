#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#define GLM_ENABLE_EXPERIMENTAL

#include "../component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

class TransformComponent : public component {
public:
	static const bool isUnique = true;

	explicit TransformComponent(TransformComponent* parentTransform = nullptr) : parent(parentTransform) {}

	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	glm::quat rotation_quat = glm::identity<glm::quat>();

	TransformComponent* parent = nullptr;

	void setPosition(const glm::vec3& newPosition) { position = newPosition; }
	glm::vec3 getPosition() const { return position; }

	void setScale(const glm::vec3& newScale) { scale = newScale; }
	glm::vec3 getScale() const { return scale; }

	void setRotation(const glm::vec3& eulerAnglesDegrees) {
		rotation_quat = glm::quat(glm::radians(eulerAnglesDegrees));
	}

	glm::vec3 getRotationEuler() const {
		return glm::degrees(glm::eulerAngles(rotation_quat));
	}

	void setRotation(const glm::quat& newRotation) { rotation_quat = newRotation; }

	glm::quat getRotation() const { return rotation_quat; }

	glm::mat4 getLocalMatrix() const;
	glm::mat4 getWorldMatrix() const;
};

#endif // TRANSFORM_COMPONENT_H