#ifndef COLLIDER_COMPONENT_H
#define COLLIDER_COMPONENT_H

#include "../component.h"

#include <variant>

class ColliderComponent : public component {
public:
    static const bool isUnique = true;

    std::variant<physics::SphereShape, physics::BoxShape, physics::CapsuleShape> shape;

    ColliderComponent(const physics::SphereShape& s) : shape(s) {}
    ColliderComponent(const physics::BoxShape& b) : shape(b) {}
    ColliderComponent(const physics::CapsuleShape& c) : shape(c) {}

    ~ColliderComponent() override;

    void addedToObject() override;
};

#endif // COLLIDER_COMPONENT_H