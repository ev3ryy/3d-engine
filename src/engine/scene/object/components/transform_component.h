#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "../component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class transformComponent : public component {
public:
	static const bool isUnique = true;

	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4 getLocalMatrix() const;
	glm::mat4 getWorldMatrix() const;
};

#endif // TRANSFORM_COMPONENT_H