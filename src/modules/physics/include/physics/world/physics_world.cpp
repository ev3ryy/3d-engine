#include "physics_world.h"
#include "../components/components.h"

#include <algorithm>
#include <cmath>

PhysicsWorld::PhysicsWorld(const glm::vec3& gravity) : m_gravity(gravity) {}

entt::entity PhysicsWorld::createSphere(const glm::vec3& position, float radius, float mass) {
    auto entity = m_registry.create();
    m_registry.emplace<TransformComponent>(entity, position);
    m_registry.emplace<SphereColliderComponent>(entity, radius);

    auto& body = m_registry.emplace<RigidBodyComponent>(entity);
    body.setMass(mass);

    if (body.inverseMass != 0.0f) {
        float i = 2.0f / 5.0f * mass * radius * radius;
        body.inverseInertiaTensor = glm::mat3(1.0f / i);
    }
    else {
        body.inverseInertiaTensor = glm::mat3(0.0f);
    }

    return entity;
}

void PhysicsWorld::step(float dt) {
    auto view = m_registry.view<RigidBodyComponent>();
    for (auto entity : view) {
        auto& body = view.get<RigidBodyComponent>(entity);
        body.forceAccumulator = { 0.0f, 0.0f, 0.0f };
        body.torqueAccumulator = { 0.0f, 0.0f, 0.0f };
    }

    applyForces();

    integrate(dt);

    detectCollisions();

    resolveCollisions();
}

void PhysicsWorld::applyForces() {
    auto view = m_registry.view<RigidBodyComponent>();
    for (auto entity : view) {
        auto& body = view.get<RigidBodyComponent>(entity);
        if (body.inverseMass == 0.0f) {
            continue;
        }
        body.forceAccumulator += m_gravity * body.mass;
    }
}

void PhysicsWorld::integrate(float dt) {
    auto view = m_registry.view<TransformComponent, RigidBodyComponent>();
    view.each([&](TransformComponent& transform, RigidBodyComponent& body) {
        if (body.inverseMass == 0.0f) {
            return;
        }

        glm::vec3 linearAcceleration = body.forceAccumulator * body.inverseMass;
        body.velocity += linearAcceleration * dt;
        transform.position += body.velocity * dt;

        glm::vec3 angularAcceleration = body.inverseInertiaTensor * body.torqueAccumulator;
        body.angularVelocity += angularAcceleration * dt;

        if (glm::length(body.angularVelocity) > 0.0001f) {
            transform.rotation += 0.5f * glm::quat(0.0f, body.angularVelocity * dt) * transform.rotation;
            transform.rotation = glm::normalize(transform.rotation);
        }
    });
}

void PhysicsWorld::detectCollisions() {
    m_contacts.clear();
    auto view = m_registry.view<TransformComponent, RigidBodyComponent, SphereColliderComponent>();

    view.each([&](const auto entityA, TransformComponent& transformA, RigidBodyComponent& bodyA, SphereColliderComponent& colliderA) {
        view.each([&](const auto entityB, TransformComponent& transformB, RigidBodyComponent& bodyB, SphereColliderComponent& colliderB) {
            if (entityA >= entityB) {
                return;
            }

            if (bodyA.inverseMass == 0.0f && bodyB.inverseMass == 0.0f) {
                return;
            }

            glm::vec3 distVec = transformB.position - transformA.position;
            float distSquared = glm::dot(distVec, distVec);
            float sumRadii = colliderA.radius + colliderB.radius;

            if (distSquared < sumRadii * sumRadii) {
                float dist = sqrt(distSquared);
                Contact contact;
                contact.bodyA = entityA;
                contact.bodyB = entityB;
                contact.penetrationDepth = sumRadii - dist;
                contact.contactNormal = (dist > 0.0f) ? distVec / dist : glm::vec3(0.0f, 1.0f, 0.0f);
                m_contacts.push_back(contact);
            }
            });
        });
}

void PhysicsWorld::resolveCollisions() {
    for (const auto& contact : m_contacts) {
        auto& transformA = m_registry.get<TransformComponent>(contact.bodyA);
        auto& bodyA = m_registry.get<RigidBodyComponent>(contact.bodyA);
        auto& transformB = m_registry.get<TransformComponent>(contact.bodyB);
        auto& bodyB = m_registry.get<RigidBodyComponent>(contact.bodyB);

        const float percent = 0.8f;
        const float slop = 0.01f;
        glm::vec3 correction = std::max(contact.penetrationDepth - slop, 0.0f) / (bodyA.inverseMass + bodyB.inverseMass) * percent * contact.contactNormal;
        if (bodyA.inverseMass != 0.0f) transformA.position -= bodyA.inverseMass * correction;
        if (bodyB.inverseMass != 0.0f) transformB.position += bodyB.inverseMass * correction;

        glm::vec3 relativeVelocity = bodyB.velocity - bodyA.velocity;
        float velAlongNormal = glm::dot(relativeVelocity, contact.contactNormal);

        if (velAlongNormal > 0) continue;

        float e = std::min(bodyA.restitution, bodyB.restitution);

        float j = -(1 + e) * velAlongNormal;
        j /= bodyA.inverseMass + bodyB.inverseMass;

        glm::vec3 impulse = j * contact.contactNormal;
        if (bodyA.inverseMass != 0.0f) bodyA.velocity -= bodyA.inverseMass * impulse;
        if (bodyB.inverseMass != 0.0f) bodyB.velocity += bodyB.inverseMass * impulse;
    }
}

glm::vec3 PhysicsWorld::getPosition(entt::entity entity) const {
    return m_registry.get<TransformComponent>(entity).position;
}

glm::quat PhysicsWorld::getRotation(entt::entity entity) const {
    return m_registry.get<TransformComponent>(entity).rotation;
}

void PhysicsWorld::setPosition(entt::entity entity, const glm::vec3& position) {
    if (m_registry.valid(entity)) {
        m_registry.get<TransformComponent>(entity).position = position;
    }
}

void PhysicsWorld::applyForce(entt::entity entity, const glm::vec3& force) {
    if (m_registry.valid(entity)) {
        m_registry.get<RigidBodyComponent>(entity).forceAccumulator += force;
    }
}

void PhysicsWorld::applyImpulse(entt::entity entity, const glm::vec3& impulse) {
    if (m_registry.valid(entity)) {
        auto& body = m_registry.get<RigidBodyComponent>(entity);
        if (body.inverseMass != 0.0f) {
            body.velocity += impulse * body.inverseMass;
        }
    }
}