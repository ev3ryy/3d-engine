#ifndef COLLIDER_COMPONENT_H
#define COLLIDER_COMPONENT_H

#include "../component.h"

#include <btBulletCollisionCommon.h>

#include <logs.h>

class ColliderComponent : public component {
public:
    static const bool isUnique = true;

    ColliderComponent() {}
    virtual ~ColliderComponent() {
        if (m_Shape) {
            LOG_INFO("ColliderComponent: Deleting btCollisionShape.");
            delete m_Shape;
            m_Shape = nullptr;
        }
        else {
            LOG_WARN("ColliderComponent: Destructor called, but m_Shape was already null.");
        }
    }

    virtual btCollisionShape* createBulletShape() const = 0;

    btCollisionShape* GetShape() { return m_Shape; }

protected:
    btCollisionShape* m_Shape = nullptr;
};

#endif // COLLIDER_COMPONENT_H