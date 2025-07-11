#ifndef PHYSICS_PHYSICS_H
#define PHYSICS_PHYSICS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdint>
#include <memory>

class PhysicsWorld;

using BodyHandle = uint32_t;

class Physics {
public:
	Physics();
	~Physics();

	void update(float dt);

	BodyHandle createSphere(const glm::vec3& position, float radius, float mass);

	void destroyBody(BodyHandle handle);

	glm::vec3 getPosition(BodyHandle handle) const;
	glm::quat getRotation(BodyHandle handle) const;

	void setPosition(BodyHandle handle, const glm::vec3& position);
	void applyForce(BodyHandle handle, const glm::vec3& force);
	void applyImpulse(BodyHandle handle, const glm::vec3& impulse);

private:
	std::unique_ptr<PhysicsWorld> m_world;
};

#endif // PHYSICS_PHYSICS_H