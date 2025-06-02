#ifndef RENDERER_RENDERER_H
#define RENDERER_RENDERER_H

class window;
class pipeline;
class World;

class renderer {
public:
	renderer();
	~renderer();

	void render(const World& world);
	void waitDeviceIdle() const;

	pipeline* getPipeline() const { return _pipeline; }

private:
	void init();

	window* _window;
	pipeline* _pipeline;
};

#endif // RENDERER_RENDERER_H