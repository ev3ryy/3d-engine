#include "transform_component.h"

glm::mat4 TransformComponent::getLocalMatrix() const
{
    glm::mat4 transformMatrix = glm::mat4(1.0f);
    transformMatrix = glm::translate(transformMatrix, position);
    transformMatrix = transformMatrix * glm::toMat4(rotation_quat);
    transformMatrix = transformMatrix * glm::scale(glm::mat4(1.0f), scale);
    return transformMatrix;
}

glm::mat4 TransformComponent::getWorldMatrix() const
{
    if (parent) {
        return parent->getWorldMatrix() * getLocalMatrix();
    }
    else {
        return getLocalMatrix();
    }
}
