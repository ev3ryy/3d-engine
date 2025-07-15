#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <btBulletDynamicsCommon.h>
#include <vector>

#include <memory>

class RigidBodyComponent;
class BulletDebugDrawer;
class Object;

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void Update(float deltaTime);

    void AddRigidBody(RigidBodyComponent* body);
    void RemoveRigidBody(RigidBodyComponent* body);

    btDiscreteDynamicsWorld* GetWorld() { return m_DynamicsWorld; }

    BulletDebugDrawer* getDebugDrawer() const { return drawer.get(); }

    void debugDrawObjectCollider(btRigidBody* body);
    void debugDrawAllEnabledColliders(const std::vector<Object*>& allObjects);

private:
    btDefaultCollisionConfiguration* m_CollisionConfiguration;
    btCollisionDispatcher* m_Dispatcher;
    btBroadphaseInterface* m_Broadphase;
    btSequentialImpulseConstraintSolver* m_Solver;
    btDiscreteDynamicsWorld* m_DynamicsWorld;

    std::vector<RigidBodyComponent*> m_RigidBodies;

    std::unique_ptr<BulletDebugDrawer> drawer;
};

#endif // PHYSICS_WORLD_H