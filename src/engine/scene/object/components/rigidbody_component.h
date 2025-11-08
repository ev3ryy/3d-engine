#ifndef RIGIDBODY_COMPONENT_H
#define RIGIDBODY_COMPONENT_H

#include "../component.h"
#include <glm/glm.hpp>

#include "../../../physics/utils/motion_state.h"

#include <logs.h>

class PhysicsWorld;
class btRigidBody;

class RigidBodyComponent : public component {
public:
    static const bool isUnique = true;

    float mass = 1.0f;

    bool isDirty = true;
    bool showColliderDebug = false;

    RigidBodyComponent() {
        m_prevPhysicsPosition = glm::vec3(0.0f);
        m_prevPhysicsRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        LOG_INFO("RigidBodyComponent created (default constructor).");
    }

    ~RigidBodyComponent() override {
        LOG_INFO("RigidBodyComponent destructor called for object: %s", getOwner()->getName().c_str());
        if (m_PhysicsWorld) {
            destroyBody(m_PhysicsWorld);
        }
        else {
            LOG_WARN("RigidBodyComponent destructor: m_PhysicsWorld was null. Body not explicitly destroyed from world.");
  
            if (m_BtRigidBody) {
                if (m_MotionState) {
                    delete m_MotionState;
                    m_MotionState = nullptr;
                }
                delete m_BtRigidBody;
                m_BtRigidBody = nullptr;
            }

            if (m_CollisionShape) {
                delete m_CollisionShape;
                m_CollisionShape = nullptr;
            }
        }
    }

    void update(float dt) override;
    void addedToObject() override;

    void initialize(PhysicsWorld* world);
    void destroyBody(PhysicsWorld* world);

    void updatePhysicsProperties();

    btRigidBody* GetBtRigidBody() { return m_BtRigidBody; }

    glm::vec3 getCurrentPhysicsPosition() const;
    glm::quat getCurrentPhysicsRotation() const;
    glm::vec3 getPreviousPhysicsPosition() const;
    glm::quat getPreviousPhysicsRotation() const;

    void setPrevPhysicsPosition(glm::vec3 value) { m_prevPhysicsPosition = value; }
    void setPrevPhysicsRotation(glm::quat value) { m_prevPhysicsRotation = value; }

private:
    btRigidBody* m_BtRigidBody = nullptr;
    MotionState* m_MotionState = nullptr;
    PhysicsWorld* m_PhysicsWorld = nullptr;
    btCollisionShape* m_CollisionShape = nullptr;

    glm::vec3 m_prevPhysicsPosition;
    glm::quat m_prevPhysicsRotation;
};

#endif // RIGIDBODY_COMPONENT_H