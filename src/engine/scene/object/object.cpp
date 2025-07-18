#include "object.h"

#include "../world/world.h"
#include "components/rigidbody_component.h"

#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include <logs.h>

int Object::nextID;

Object::Object(const std::string& name) : name(name)
{
    id = nextID++;
    addComponent<TransformComponent>();

    canUpdate = true;
}

Object::~Object()
{
}

void Object::update(float deltaTime)
{
    for (const auto& child : children) {
        if (child->canUpdate) {
            child->update(deltaTime);
        }
    }

    for (const auto& component : components) {
        if (component->canUpdate) {
            component->update(deltaTime);
        }
    }
}

const std::string& Object::getName() const
{
    return name;
}

int Object::getID() const
{
    return id;
}

void Object::setName(const std::string& newName) {
    name = newName;
}

void Object::setWorld(World* world)
{
    m_world = world;
}

void Object::addChild(std::unique_ptr<Object> child) {
    if (!child) {
        return;
    }

    if (child->parent != nullptr) {
        child->parent->detachChild(child.get());
    }

    child->parent = this;

    auto childTransform = child->getComponent<TransformComponent>();
    auto parentTransform = this->getComponent<TransformComponent>();

    if (childTransform && parentTransform) {
        childTransform->parent = parentTransform;
    }
    else {
        LOG_ERROR("Object '%s': Transform Component is null for child '%s' or parent '%s'.",
            name.c_str(), child->getName().c_str(), this->getName().c_str());
    }

    children.push_back(std::move(child));
}

void Object::removeChild(Object* child) {
    if (!child) return;

    auto it = std::remove_if(children.begin(), children.end(),
        [child](const std::unique_ptr<Object>& obj) { return obj.get() == child; });

    if (it != children.end()) {
        if (child->parent == this) {
            child->parent = nullptr;
            if (child->getComponent<TransformComponent>()) {
                child->getComponent<TransformComponent>()->parent = nullptr;
            }
        }
        children.erase(it, children.end());
    }
}

std::unique_ptr<Object> Object::detachChild(Object* child) {
    if (!child) return nullptr;

    std::unique_ptr<Object> detachedChild = nullptr;
    auto it = std::find_if(children.begin(), children.end(),
        [child](const std::unique_ptr<Object>& obj) { return obj.get() == child; });

    if (it != children.end()) {
        detachedChild = std::move(*it);
        children.erase(it);

        detachedChild->parent = nullptr;
        if (detachedChild->getComponent<TransformComponent>()) {
            detachedChild->getComponent<TransformComponent>()->parent = nullptr;
        }
    }
    return detachedChild;
}

std::unique_ptr<Object> Object::deepCopy() const {
    auto newObject = std::make_unique<Object>(this->name);

    if (const auto* sourceTransform = this->getComponent<TransformComponent>()) {
        auto* destTransform = newObject->getComponent<TransformComponent>();
        destTransform->position = sourceTransform->position;
        destTransform->rotation_quat = sourceTransform->rotation_quat;
        destTransform->scale = sourceTransform->scale;
    }

    if (const auto* sourceMeshRenderer = this->getComponent<MeshRendererComponent>()) {
        newObject->addComponent<MeshRendererComponent>(sourceMeshRenderer->getMesh(), sourceMeshRenderer->getMaterialID());
    }

    for (const auto& child : this->children) {
        if (child) {
            newObject->addChild(child->deepCopy());
        }
    }

    return newObject;
}
