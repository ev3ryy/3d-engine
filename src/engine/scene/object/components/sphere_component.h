#ifndef SPHERE_COMPONENT_H
#define SPHERE_COMPONENT_H

#include "collider_component.h"

#include "rigidbody_component.h"
#include <logs.h>

class SphereComponent : public ColliderComponent {
public:
    SphereComponent(float radius) : radius(radius) {
        m_Shape = new btSphereShape(radius);
        LOG_INFO("SphereComponent created with radius: %.2f", radius);
    }

    float getRadius() const { return radius; }
    void setRadius(float value) {
        if (radius != value) {
            radius = value;
            if (m_Shape) {
                delete m_Shape;
                m_Shape = nullptr;
            }
            m_Shape = new btSphereShape(radius);
            LOG_INFO("SphereComponent radius changed to %.2f. New btSphereShape created.", radius);

            if (getOwner()) {
                if (RigidBodyComponent* rb = getOwner()->getComponent<RigidBodyComponent>()) {
                    rb->isDirty = true;
                }
            }
        }
    }

    btCollisionShape* createBulletShape() const override {
        return new btSphereShape(radius);
    }

private:
    float radius = 1.0f;
};

#endif // SPHERE_COMPONENT_H