#ifndef RENDERER_WINDOW_H
#define RENDERER_WINDOW_H

#include <cstdint>

struct GLFWwindow;

struct PerformanceStats {
	float fps;
	float avgFrameTime;
};

class window {
public:
	window(const uint32_t width, const uint32_t height, const char* title);
	window() = delete;

	~window();

	static PerformanceStats updatePerfomanceStats();

	static float getFps();
	static float getAvgFrameTime();

	static GLFWwindow* _window;

	static bool framebufferResized;

private:
	void init(const uint32_t width, const uint32_t height, const char* title);
	static void framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
};

#endif // RENDERER_WINDOW_H