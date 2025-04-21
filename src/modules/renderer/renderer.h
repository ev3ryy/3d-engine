#ifndef RENDERER_RENDERER_H
#define RENDERER_RENDERER_H

class window;
class pipeline;

class renderer {
public:
	renderer();
	~renderer();

	pipeline* getPipeline() const { return _pipeline; }

private:
	void init();

	window* _window;
	pipeline* _pipeline;
};

#endif // RENDERER_RENDERER_H