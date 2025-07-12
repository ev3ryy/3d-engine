#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <btBulletDynamicsCommon.h>
#include <vector>

class RigidBodyComponent;

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void Update(float deltaTime);

    void AddRigidBody(RigidBodyComponent* body);
    void RemoveRigidBody(RigidBodyComponent* body);

    btDiscreteDynamicsWorld* GetWorld() { return m_DynamicsWorld; }

private:
    btDefaultCollisionConfiguration* m_CollisionConfiguration;
    btCollisionDispatcher* m_Dispatcher;
    btBroadphaseInterface* m_Broadphase;
    btSequentialImpulseConstraintSolver* m_Solver;
    btDiscreteDynamicsWorld* m_DynamicsWorld;

    std::vector<RigidBodyComponent*> m_RigidBodies;
};

#endif // PHYSICS_WORLD_H