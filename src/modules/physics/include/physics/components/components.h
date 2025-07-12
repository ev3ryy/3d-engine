#ifndef PHYSICS_COMPONENTS_H
#define PHYSICS_COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct TransformComponent {
    glm::vec3 position{ 0.0f };

    glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
};

struct RigidBody {
    glm::vec3 velocity{ 0.0f };
    glm::vec3 forceAccumulator{ 0.0f };

    glm::vec3 angularVelocity{ 0.0f };
    glm::vec3 torqueAccumulator{ 0.0f };

    float mass = 1.0f;
    float inverseMass = 1.0f;
    glm::mat3 inverseInertiaTensor{ 1.0f };

    float restitution = 0.6f;
    float friction = 0.5f;

    void setMass(float newMass) {
        mass = newMass;
        if (mass <= 0.0001f) {
            inverseMass = 0.0f;
        }
        else {
            inverseMass = 1.0f / mass;
        }
    }
};

struct SphereColliderComponent {
    float radius = 1.0f;

    SphereColliderComponent(float r) : radius(r) {}
};

#endif // PHYSICS_COMPONENTS_H