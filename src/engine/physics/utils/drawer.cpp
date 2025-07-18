#include "drawer.h"
#include <logs.h>

BulletDebugDrawer::BulletDebugDrawer() : m_debugMode(DBG_DrawWireframe) {
}

BulletDebugDrawer::~BulletDebugDrawer() {
}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    glm::vec3 glmFrom = glm::vec3(from.getX(), from.getY(), from.getZ());
    glm::vec3 glmTo = glm::vec3(to.getX(), to.getY(), to.getZ());
    glm::vec3 glmColor = glm::vec3(color.getX(), color.getY(), color.getZ());

    uint32_t startIndex = static_cast<uint32_t>(m_vertices.size());
    m_vertices.push_back({ glmFrom, glmColor });
    m_vertices.push_back({ glmTo, glmColor });
    m_indices.push_back(startIndex);
    m_indices.push_back(startIndex + 1);
}

void BulletDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {

}

void BulletDebugDrawer::reportErrorWarning(const char* warningString) {
    LOG_WARN("Bullet Debug Warning: %s", warningString);
}

void BulletDebugDrawer::draw3dText(const btVector3& location, const char* textString) {

}

void BulletDebugDrawer::setDebugMode(int debugMode) {
    m_debugMode = debugMode;
}

int BulletDebugDrawer::getDebugMode() const {
    return m_debugMode;
}

void BulletDebugDrawer::clearLines() {
    m_vertices.clear();
    m_indices.clear();
}