#include "rigidbody_component.h"
#include "../object.h"
#include "../../world/world.h"
#include "transform_component.h"
#include <stdexcept>

RigidBodyComponent::RigidBodyComponent() {}

RigidBodyComponent::~RigidBodyComponent() {
    if (m_physicsFacade && m_bodyHandle != -1) {
        m_physicsFacade->destroyBody(m_bodyHandle);
    }
}

void RigidBodyComponent::update(float dt)
{
    if (isDirty) {
        if (m_physicsFacade && m_bodyHandle != -1) {
            m_physicsFacade->destroyBody(m_bodyHandle);
        }

        addedToObject();
        isDirty = false;
    }
}

void RigidBodyComponent::setPhysicsFacade(Physics* facade) {
    m_physicsFacade = facade;
}

void RigidBodyComponent::addedToObject() {
    if (getOwner() && getOwner()->getWorld()) {
        setPhysicsFacade(getOwner()->getWorld()->getPhysicsFacade());
    }

    if (!m_physicsFacade) {
        throw std::runtime_error("Physics facade is not set for RigidBodyComponent!");
    }

    transformComponent* transform = getOwner()->getTransform();
    if (!transform) {
        throw std::runtime_error("Object must have a transformComponent to have a RigidBodyComponent!");
    }

    m_bodyHandle = m_physicsFacade->createSphere(transform->position, sphereRadius, mass);
}