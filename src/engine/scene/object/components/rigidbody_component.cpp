#include "rigidbody_component.h"
#include "../object.h"
#include "transform_component.h"
#include "collider_component.h"
#include "physics/world/physics_world.h"

void RigidBodyComponent::addedToObject() {
    isDirty = true;
    LOG_INFO("RigidBodyComponent added to object: %s. Marked as dirty.", getOwner()->getName().c_str());
}

void RigidBodyComponent::update(float dt) {
    if (isDirty && m_PhysicsWorld) {
        LOG_WARN("RigidBodyComponent::update - Object %s is dirty, applying changes to physics body.", getOwner()->getName().c_str());
        if (!m_BtRigidBody) {
            initialize(m_PhysicsWorld);
        }
        else {
            updatePhysicsProperties();
        }
        isDirty = false;
    }

    if (m_BtRigidBody && m_BtRigidBody->getMotionState()) {
        btTransform trans;
        m_BtRigidBody->getMotionState()->getWorldTransform(trans);

        TransformComponent* transform = getOwner()->getComponent<TransformComponent>();
        if (transform) {
            transform->position.x = trans.getOrigin().getX();
            transform->position.y = trans.getOrigin().getY();
            transform->position.z = trans.getOrigin().getZ();

            btQuaternion rot = trans.getRotation();
            transform->setRotation(glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ()));
        }
    }
}

void RigidBodyComponent::initialize(PhysicsWorld* world) {
    m_PhysicsWorld = world;

    LOG_INFO("RigidBodyComponent::initialize for object: %s", getOwner()->getName().c_str());

    if (m_BtRigidBody) {
        LOG_WARN("RigidBodyComponent::initialize - Existing body found, destroying it first.");
        destroyBody(world);
    }

    TransformComponent* transform = getOwner()->getComponent<TransformComponent>();
    ColliderComponent* collider = getOwner()->getComponent<ColliderComponent>();

    if (!transform || !collider) {
        LOG_ERROR("RigidBodyComponent::initialize - Object %s requires a TransformComponent and a Collider component.", getOwner()->getName().c_str());
        m_PhysicsWorld = nullptr;
        return;
    }

    m_CollisionShape = collider->createBulletShape();
    if (!m_CollisionShape) {
        LOG_ERROR("RigidBodyComponent::initialize - ColliderComponent on object %s returned a null shape. Cannot create RigidBody.", getOwner()->getName().c_str());
        m_PhysicsWorld = nullptr;
        return;
    }

    m_MotionState = new MotionState(transform);
    LOG_WARN("RigidBodyComponent::initialize - MotionState created.");

    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f) {
        m_CollisionShape->calculateLocalInertia(mass, localInertia);
        LOG_WARN("RigidBodyComponent::initialize - Calculated local inertia for mass %.2f: (%.2f, %.2f, %.2f)", mass, localInertia.x(), localInertia.y(), localInertia.z());
    }
    else {
        LOG_INFO("RigidBodyComponent::initialize - Mass is 0.0f, creating a static/kinematic body.");
    }

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_MotionState, m_CollisionShape, localInertia);
    m_BtRigidBody = new btRigidBody(rbInfo);

#ifdef _DEBUG
    m_BtRigidBody->setActivationState(DISABLE_DEACTIVATION);
#endif

    world->GetWorld()->addRigidBody(m_BtRigidBody);
    isDirty = false;
    LOG_INFO("RigidBodyComponent::initialize - Successfully created and added rigid body for object: %s.", getOwner()->getName().c_str());
}

void RigidBodyComponent::destroyBody(PhysicsWorld* world) {
    if (m_BtRigidBody) {
        LOG_INFO("RigidBodyComponent::destroyBody - Removing rigid body for object: %s from physics world.", getOwner()->getName().c_str());
        world->GetWorld()->removeRigidBody(m_BtRigidBody);

        if (m_MotionState) {
            LOG_WARN("RigidBodyComponent::destroyBody - Deleting MotionState.");
            delete m_MotionState;
            m_MotionState = nullptr;
        }

        LOG_WARN("RigidBodyComponent::destroyBody - Deleting btRigidBody.");
        delete m_BtRigidBody;
        m_BtRigidBody = nullptr;
        LOG_INFO("RigidBodyComponent::destroyBody - Rigid body for object: %s successfully destroyed.", getOwner()->getName().c_str());
    }
    else {
        LOG_WARN("RigidBodyComponent::destroyBody - No btRigidBody to destroy for object: %s.", getOwner()->getName().c_str());
    }

    if (m_CollisionShape) {
        LOG_WARN("RigidBodyComponent::destroyBody - Deleting btCollisionShape.");
        delete m_CollisionShape;
        m_CollisionShape = nullptr;
    }
}

void RigidBodyComponent::updatePhysicsProperties()
{
    if (!m_BtRigidBody || !getOwner() || !m_PhysicsWorld) {
        LOG_ERROR("RigidBodyComponent::updatePhysicsProperties called with invalid state. Body, owner or physics world is null.");
        return;
    }

    ColliderComponent* collider = getOwner()->getComponent<ColliderComponent>();
    if (!collider) {
        LOG_ERROR("RigidBodyComponent::updatePhysicsProperties - No ColliderComponent found on object %s.", getOwner()->getName().c_str());
        return;
    }

    btCollisionShape* newShape = collider->createBulletShape();
    if (!newShape) {
        LOG_ERROR("RigidBodyComponent::updatePhysicsProperties - ColliderComponent on object %s returned a null shape. Cannot update RigidBody.", getOwner()->getName().c_str());
        return;
    }

    if (m_CollisionShape) {
        LOG_INFO("RigidBodyComponent::updatePhysicsProperties - Deleting old btCollisionShape.");
        delete m_CollisionShape;
    }
    m_CollisionShape = newShape;

    LOG_INFO("RigidBodyComponent::updatePhysicsProperties - Updating collision shape for object: %s.", getOwner()->getName().c_str());
    m_BtRigidBody->setCollisionShape(m_CollisionShape);


    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f) {
        m_CollisionShape->calculateLocalInertia(mass, localInertia);
        LOG_INFO("RigidBodyComponent::updatePhysicsProperties - Recalculated local inertia for mass %.2f: (%.2f, %.2f, %.2f)", mass, localInertia.x(), localInertia.y(), localInertia.z());
    }
    else {
        LOG_INFO("RigidBodyComponent::updatePhysicsProperties - Mass is 0.0f, body will be static/kinematic.");
    }
    m_BtRigidBody->setMassProps(mass, localInertia);
    m_BtRigidBody->updateInertiaTensor();

    if (mass > 0.0f && (m_BtRigidBody->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT)) {
        m_BtRigidBody->setCollisionFlags(m_BtRigidBody->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
    }
    else if (mass == 0.0f && !(m_BtRigidBody->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT)) {
        m_BtRigidBody->setCollisionFlags(m_BtRigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
    }

    m_BtRigidBody->activate(true);
    LOG_INFO("RigidBodyComponent::updatePhysicsProperties - Physics properties updated for object: %s.", getOwner()->getName().c_str());
}

glm::vec3 RigidBodyComponent::getCurrentPhysicsPosition() const {
    TransformComponent* transform = getOwner()->getComponent<TransformComponent>();
    return transform ? transform->getPosition() : glm::vec3(0.0f);
}

glm::quat RigidBodyComponent::getCurrentPhysicsRotation() const {
    TransformComponent* transform = getOwner()->getComponent<TransformComponent>();
    return transform ? transform->getRotation() : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

glm::vec3 RigidBodyComponent::getPreviousPhysicsPosition() const {
    return m_prevPhysicsPosition;
}

glm::quat RigidBodyComponent::getPreviousPhysicsRotation() const {
    return m_prevPhysicsRotation;
}