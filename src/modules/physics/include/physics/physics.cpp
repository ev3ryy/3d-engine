#include "physics.h"
#include "logs/logs.h"

#include "components/components.h"
#include "world/physics_world.h"

#include <iostream>
#include <vector>
#include <iomanip>

Physics::Physics() {
    m_world = std::make_unique<PhysicsWorld>();
}

Physics::~Physics() = default;

void Physics::update(float dt) {
    m_world->step(dt);
}

BodyHandle Physics::createSphere(const glm::vec3& position, float radius, float mass) {
    entt::entity entity = m_world->createSphere(position, radius, mass);
    return static_cast<BodyHandle>(entity);
}

void Physics::destroyBody(BodyHandle handle) {
    m_world->getRegistry().destroy(static_cast<entt::entity>(handle));
}

glm::vec3 Physics::getPosition(BodyHandle handle) const {
    return m_world->getPosition(static_cast<entt::entity>(handle));
}

glm::quat Physics::getRotation(BodyHandle handle) const {
    return m_world->getRotation(static_cast<entt::entity>(handle));
}

void Physics::setPosition(BodyHandle handle, const glm::vec3& position) {
    m_world->setPosition(static_cast<entt::entity>(handle), position);
}

void Physics::applyForce(BodyHandle handle, const glm::vec3& force) {
    m_world->applyForce(static_cast<entt::entity>(handle), force);
}

void Physics::applyImpulse(BodyHandle handle, const glm::vec3& impulse) {
    m_world->applyImpulse(static_cast<entt::entity>(handle), impulse);
}

//int main() {
//    Physics physicsFacade;
//
//    auto fallingBall = physicsFacade.createSphere({ 0.0f, 20.0f, 0.0f }, 1.0f, 2.0f);
//    auto staticFloor = physicsFacade.createSphere({ 0.0f, -50.0f, 0.0f }, 50.0f, 0.0f);
//
//    LOG_INFO("Starting simulation...");
//
//    const float fixedTimeStep = 1.0f / 60.0f;
//    const int simulationSteps = 300;
//
//    for (int i = 0; i < simulationSteps; ++i) {
//        physicsFacade.update(fixedTimeStep);
//
//        if (i % 30 == 0) {
//            float time = i * fixedTimeStep;
//            glm::vec3 ballPosition = physicsFacade.getPosition(fallingBall);
//
//            std::cout << std::fixed << std::setprecision(2);
//            std::cout << "Time: " << time << "s, Ball Position: Y = " << ballPosition.y << std::endl;
//        }
//    }
//
//    glm::vec3 finalPosition = physicsFacade.getPosition(fallingBall);
//    std::cout << "Simulation finished. Final ball position: Y = " << finalPosition.y << std::endl;
//}