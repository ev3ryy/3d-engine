#include "window.h"

#include <vulkan/pipeline.h>

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

GLFWwindow* window::_window;
bool window::framebufferResized = false;

window::window(const uint32_t width, const uint32_t height, const char* title)
{
	init(width, height, title);
}

window::~window()
{
	glfwDestroyWindow(_window);
	glfwTerminate();

	LOG_INFO("glfw terminate");
}

PerformanceStats window::updatePerfomanceStats()
{
	static double previousTime = glfwGetTime();
	static int frameCount = 0;
	static float avgFrameTimeMS = 0;
	static float fps = 0.0f;

	double currentTime = glfwGetTime();
	frameCount++;
	double elapsedTime = currentTime - previousTime;

	if (elapsedTime >= 1.0)
	{
		fps = static_cast<float>(frameCount) / static_cast<float>(elapsedTime);
		avgFrameTimeMS = 1000.0f / fps;
		frameCount = 0;
		previousTime = currentTime;
	}

	return {fps, avgFrameTimeMS};
}

float window::getFps()
{
	return updatePerfomanceStats().fps;
}

float window::getAvgFrameTime()
{
	return updatePerfomanceStats().avgFrameTime;
}

void window::init(const uint32_t width, const uint32_t height, const char* title)
{
	if (!glfwInit()) {
		LOG_CRITICAL("failed to initialize glfw");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!_window) {
		LOG_CRITICAL("failed to create window");
	}

	glfwSetWindowUserPointer(_window, this);
	glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);

	LOG_INFO("window: initialized!");
}

void window::framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
	auto wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfwWindow));
	if (wnd) {
		wnd->framebufferResized = true;
	}
}
