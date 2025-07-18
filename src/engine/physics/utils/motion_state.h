#ifndef PHYSICS_MOTIONSTATE_H
#define PHYSICS_MOTIONSTATE_H

#include "../../scene/object/components/transform_component.h"
#include <btBulletDynamicsCommon.h>

#include <glm/gtc/quaternion.hpp>

#include <logs.h>

#include "../../scene/object/object.h"

class MotionState : public btMotionState {
public:
    MotionState(TransformComponent* transform) : m_Transform(transform) {}

    void getWorldTransform(btTransform& worldTrans) const override {
        glm::vec3 pos = m_Transform->getPosition();
        glm::quat rot = m_Transform->getRotation();

        worldTrans.setOrigin(btVector3(pos.x, pos.y, pos.z));
        worldTrans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

        //LOG_INFO("MotionState::getWorldTransform - Object: %s, Position: (%.2f, %.2f, %.2f), Rotation: (%.2f, %.2f, %.2f, %.2f)",
        //    m_Transform->getOwner()->getName().c_str(), pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, rot.w);
    }

    void setWorldTransform(const btTransform& worldTrans) override {
        btVector3 pos = worldTrans.getOrigin();
        btQuaternion rot = worldTrans.getRotation();

        m_Transform->setPosition(glm::vec3(pos.x(), pos.y(), pos.z()));
        m_Transform->setRotation(glm::quat(rot.w(), rot.x(), rot.y(), rot.z()));

        //LOG_INFO("MotionState::setWorldTransform - Object: %s, Bullet Pos: (%.2f, %.2f, %.2f), Bullet Rot: (%.2f, %.2f, %.2f, %.2f)",
        //    m_Transform->getOwner()->getName().c_str(), pos.x(), pos.y(), pos.z(), rot.x(), rot.y(), rot.z(), rot.w());
    }

private:
    TransformComponent* m_Transform;
};

#endif // PHYSICS_MOTIONSTATE_H