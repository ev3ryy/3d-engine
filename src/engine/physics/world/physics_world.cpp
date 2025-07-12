#include "physics_world.h"

#include "../../scene/object/components/rigidbody_component.h"

#include <logs.h>

PhysicsWorld::PhysicsWorld() {
    m_CollisionConfiguration = new btDefaultCollisionConfiguration();
    m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);
    m_Broadphase = new btDbvtBroadphase();
    m_Solver = new btSequentialImpulseConstraintSolver();
    m_DynamicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_CollisionConfiguration);

    m_DynamicsWorld->setGravity(btVector3(0, -9.81, 0));
}

PhysicsWorld::~PhysicsWorld() {
    for (int i = m_DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_DynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        m_DynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    delete m_DynamicsWorld;
    delete m_Solver;
    delete m_Broadphase;
    delete m_Dispatcher;
    delete m_CollisionConfiguration;
}

void PhysicsWorld::Update(float deltaTime) {
    const int maxSubSteps = 10;
    const btScalar fixedTimeStep = 1.0f / 60.0f;

    m_DynamicsWorld->stepSimulation(deltaTime, maxSubSteps, fixedTimeStep);
}

void PhysicsWorld::AddRigidBody(RigidBodyComponent* body) {
    m_DynamicsWorld->addRigidBody(body->GetBtRigidBody());
    m_RigidBodies.push_back(body);
}

void PhysicsWorld::RemoveRigidBody(RigidBodyComponent* body) {
    m_DynamicsWorld->removeRigidBody(body->GetBtRigidBody());
}