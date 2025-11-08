#ifndef RENDERER_RENDERER_H
#define RENDERER_RENDERER_H

#include <glm/glm.hpp>

#include <irenderer.h>
#include <shaders/shader_manager.h>

class window;
class World;
class Mesh;
class MaterialInstance;
class ResourceManager;
class Object;
class PhysicsWorld;

struct RenderObject;

class renderer {
public:
	renderer();
	~renderer();

	void init();
	void shutdown();

	void render(const World& world, ResourceManager& resourceManager, float alpha, PhysicsWorld* physicsWorld);
	void waitDeviceIdle() const;

	void collectRenderableObjectsRecursive(Object* currentObject, std::vector<RenderObject>& renderObjects, ResourceManager& resourceManager, float interpolationAlpha);
	void syncWithWorld(const World& world, ResourceManager& resourceManager, float interpolationAlpha);

	void uploadMesh(Mesh* mesh);

	IPipeline* getPipeline() const { return pipeline; }

private:
	window* _window;
	IPipeline* pipeline = nullptr;
	std::vector<RenderObject> _renderObjects;

	std::unique_ptr<ShaderManager> shaderManager;
};

#endif // RENDERER_RENDERER_H