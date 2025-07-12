#ifndef BOX_COMPONENT_H
#define BOX_COMPONENT_H

#include "collider_component.h"

#include <logs.h>

class BoxComponent : public ColliderComponent {
public:
    BoxComponent(const glm::vec3& halfExtents) {
        m_Shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
        LOG_INFO("BoxComponent created with halfExtents: (%.2f, %.2f, %.2f)", halfExtents.x, halfExtents.y, halfExtents.z);
    }
};

#endif // BOX_COMPONENT_H