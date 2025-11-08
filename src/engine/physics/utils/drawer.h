#ifndef PHYSICS_DRAWER_H
#define PHYSICS_DRAWER_H

#include <btBulletDynamicsCommon.h>
#include <vector>
#include <glm/glm.hpp>

#include <mesh/mesh.h>

class BulletDebugDrawer : public btIDebugDraw {
public:
    BulletDebugDrawer();
    ~BulletDebugDrawer();

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
    virtual void reportErrorWarning(const char* warningString) override;
    virtual void draw3dText(const btVector3& location, const char* textString) override;
    virtual void setDebugMode(int debugMode) override;
    virtual int getDebugMode() const override;

    void clearLines() override;
    const std::vector<DebugLineVertex>& getVertices() const { return m_vertices; }
    const std::vector<uint32_t>& getIndices() const { return m_indices; }

private:
    std::vector<DebugLineVertex> m_vertices;
    std::vector<uint32_t> m_indices;
    int m_debugMode;
};

#endif // PHYSICS_DRAWER_H