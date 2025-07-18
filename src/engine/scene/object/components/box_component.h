#ifndef BOX_COMPONENT_H
#define BOX_COMPONENT_H

#include "collider_component.h"
#include "rigidbody_component.h"

#include <glm/glm.hpp>
#include <logs.h>

class BoxComponent : public ColliderComponent {
public:
    BoxComponent(const glm::vec3& halfExtents = glm::vec3(0.5f, 0.5f, 0.5f))
        : m_halfExtents(halfExtents) {
        LOG_INFO("BoxComponent created with halfExtents: (%.2f, %.2f, %.2f)", halfExtents.x, halfExtents.y, halfExtents.z);
    }

    const glm::vec3& getHalfExtents() const { return m_halfExtents; }

    void setHalfExtents(const glm::vec3& value) {
        if (m_halfExtents != value) {
            m_halfExtents = value;
            if (getOwner()) {
                if (RigidBodyComponent* rb = getOwner()->getComponent<RigidBodyComponent>()) {
                    rb->isDirty = true;
                }
            }
            LOG_INFO("BoxComponent halfExtents changed to (%.2f, %.2f, %.2f). Marked RigidBodyComponent as dirty.",
                m_halfExtents.x, m_halfExtents.y, m_halfExtents.z);
        }
    }

    btCollisionShape* createBulletShape() const override {
        return new btBoxShape(btVector3(m_halfExtents.x, m_halfExtents.y, m_halfExtents.z));
    }

private:
    glm::vec3 m_halfExtents;
};

#endif // BOX_COMPONENT_H