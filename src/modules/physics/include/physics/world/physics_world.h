#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <entt/entt.hpp>
#include <glm/glm.hpp>

struct Contact {
    entt::entity bodyA;
    entt::entity bodyB;
    glm::vec3 contactNormal;
    float penetrationDepth;
};

class PhysicsWorld {
public:
    explicit PhysicsWorld(const glm::vec3& gravity = { 0.0f, -9.81f, 0.0f });

    void step(float dt);

    entt::entity createSphere(const glm::vec3& position, float radius, float mass);

    const entt::registry& getRegistry() const { return m_registry; }
    entt::registry& getRegistry() { return m_registry; }

    glm::vec3 getPosition(entt::entity entity) const;
    glm::quat getRotation(entt::entity entity) const;
    void setPosition(entt::entity entity, const glm::vec3& position);
    void applyForce(entt::entity entity, const glm::vec3& force);
    void applyImpulse(entt::entity entity, const glm::vec3& impulse);

private:
    void applyForces();
    void integrate(float dt);
    void detectCollisions();
    void resolveCollisions();

    entt::registry m_registry;
    glm::vec3 m_gravity;
    std::vector<Contact> m_contacts;
};

#endif // PHYSICS_WORLD_H
