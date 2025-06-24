#ifndef RENDERER_RENDERER_H
#define RENDERER_RENDERER_H

#include <glm/glm.hpp>

class window;
class pipeline;
class World;
class Mesh;
class MaterialInstance;
class ResourceManager;

struct RenderObject;

class renderer {
public:
	renderer();
	~renderer();

	void render(const World& world, ResourceManager& resourceManager);
	void waitDeviceIdle() const;
	void syncWithWorld(const World& world, ResourceManager& resourceManager);

	pipeline* getPipeline() const { return _pipeline; }

private:
	void init();

	window* _window;
	pipeline* _pipeline;
	std::vector<RenderObject> _renderObjects;
};

#endif // RENDERER_RENDERER_H