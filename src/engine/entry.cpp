#include "entry.h"

#include "ui/debug/ui.h"
#include "scene/world/world.h"
#include "scene/object/components/camera_component.h"

#include <imgui.h>
#include <vulkan/imgui_impl_glfw.h>
#include <vulkan/imgui_impl_vulkan.h>

#include <core.h>
#include <renderer.h>
#include <input.h>
#include <logger.h>
#include <vulkan/pipeline.h>
#include <camera/camera.h>
#include <mesh/primitives/primitives.h>

#include <utils/keycodes.h>

#include <iostream>
#include <filesystem>
#include <sstream>

config Engine::_config;

Engine::Engine(core* coreInstance, renderer* rendererInstance)
    : _core(coreInstance), _renderer(rendererInstance)
{
    if (!_renderer) {
        throw std::runtime_error("Renderer is null");
    }
}

Engine::~Engine() {

}

void Engine::mainLoop() {
    world = std::make_unique<World>();

    auto cameraObj = world->createObject("MainCamera");
    auto cameraComp = cameraObj->addComponent<CameraComponent>(
        glm::vec3(0.0f, 0.0f, 3.0f), // position
        glm::vec3(0.0f, 1.0f, 0.0f), // up
        -90.0f,                      // yaw
        0.0f                         // pitch
    );

    world->setActiveRenderCamera(&cameraComp->camera);

    bool flag = false;;

    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

    while (!glfwWindowShouldClose(window::_window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        input::update();
        glfwPollEvents();

        world->update(deltaTime);

        int fb_width, fb_height;
        glfwGetFramebufferSize(window::_window, &fb_width, &fb_height);
        if (fb_width == 0 || fb_height == 0) {
            glfwWaitEvents();
            continue;
        }

        if (fb_width != last_fb_width || fb_height != last_fb_height) {
            last_fb_width = fb_width;
            last_fb_height = fb_height;
        }

        // start imgui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        PerformanceStats stats = window::updatePerfomanceStats();

        if (_config.mainWindow) {
            ui::debug::drawDebugMenu(*world.get(), *_renderer);
        }

        ImGui::Render();
        _renderer->render(*world.get());
    }

    _renderer->waitDeviceIdle();
}

int main() {
    logger::init();

    auto _core = std::make_unique<core>();
    auto _renderer = std::make_unique<renderer>();

    ui::debug::initialize(*_renderer); // initialize imgui debug ui

    auto engine = std::make_unique<Engine>(_core.get(), _renderer.get());
    Engine::_config.mainWindow = true;

    input::init(window::_window);

    engine->mainLoop();

    return 0;
}
