#ifndef RIGIDBODY_COMPONENT_H
#define RIGIDBODY_COMPONENT_H

#include "../component.h"
#include <physics/physics.h>
#include <glm/glm.hpp>

class Physics;

class RigidBodyComponent : public component {
public:
    static const bool isUnique = true;

    float mass = 1.0f;
    float sphereRadius = 0.5f;

    RigidBodyComponent();
    ~RigidBodyComponent() override;

    void addedToObject() override;

    void setPhysicsFacade(Physics* facade);

    BodyHandle getBodyHandle() const { return m_bodyHandle; }

private:
    Physics* m_physicsFacade = nullptr;
    BodyHandle m_bodyHandle = -1;
};

#endif // RIGIDBODY_COMPONENT_H