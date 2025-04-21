#include "renderer.h"

#include "window/window.h"
#include "vulkan/pipeline.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

renderer::renderer()
{
	init();
}

renderer::~renderer()
{
	delete _pipeline;
	delete _window;

	LOG_INFO("Renderer module: shutdown");
}

void renderer::init() {
	LOG_INFO("Renderer module: initialized");

	_window = new window(1920, 1080, "Engine");
	_pipeline = new pipeline();
}
