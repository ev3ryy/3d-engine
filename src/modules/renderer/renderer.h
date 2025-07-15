#ifndef RENDERER_RENDERER_H
#define RENDERER_RENDERER_H

#include <glm/glm.hpp>

class window;
class pipeline;
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

	void render(const World& world, ResourceManager& resourceManager, float alpha, PhysicsWorld* physicsWorld);
	void waitDeviceIdle() const;

	void collectRenderableObjectsRecursive(Object* currentObject, std::vector<RenderObject>& renderObjects, ResourceManager& resourceManager, pipeline* pipeline, float interpolationAlpha);
	void syncWithWorld(const World& world, ResourceManager& resourceManager, float interpolationAlpha);

	pipeline* getPipeline() const { return _pipeline; }

private:
	void init();

	window* _window;
	pipeline* _pipeline;
	std::vector<RenderObject> _renderObjects;
};

#endif // RENDERER_RENDERER_H