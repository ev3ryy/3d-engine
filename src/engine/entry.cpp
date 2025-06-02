#include "entry.h"

#include "ui/debug/ui.h"
#include "scene/world/world.h"

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
    world->setActiveRenderCamera(&_renderer->getPipeline()->_camera);

    bool flag = false;;

    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

    while (!glfwWindowShouldClose(window::_window)) {
        lastTime = std::chrono::high_resolution_clock::now();
        currentTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        input::update();
        glfwPollEvents();

        bool cameraControlActive = (glfwGetMouseButton(window::_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

        // move this to camera component or etc

        //if (cameraControlActive) {
        //    glfwSetInputMode(window::_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        //    bool shiftDown = input::IsKeyDown(keycode::LShift);

        //    float speedMultiplier = shiftDown ? 2.0f : 1.0f;
        //    float adjustedDeltaTime = deltaTime * speedMultiplier;

        //    if (input::IsKeyDown(keycode::W))
        //        _pipeline->_camera.ProcessKeyboard(CameraMovement::FORWARD, adjustedDeltaTime);
        //    if (input::IsKeyDown(keycode::S))
        //        _pipeline->_camera.ProcessKeyboard(CameraMovement::BACKWARD, adjustedDeltaTime);
        //    if (input::IsKeyDown(keycode::A))
        //        _pipeline->_camera.ProcessKeyboard(CameraMovement::LEFT, adjustedDeltaTime);
        //    if (input::IsKeyDown(keycode::D))
        //        _pipeline->_camera.ProcessKeyboard(CameraMovement::RIGHT, adjustedDeltaTime);

        //    float xoffset, yoffset;
        //    input::getMouseDelta(xoffset, yoffset);
        //    if (xoffset != 0.0f || yoffset != 0.0f)
        //        _pipeline->_camera.ProcessMouseMovement(xoffset, yoffset);

        //    float scrollX, scrollY;
        //    input::getScrollDelta(scrollX, scrollY);
        //    if (scrollY != 0.0f) {
        //        const float speedSensitivity = 0.1f; // Коэффициент изменения скорости
        //        _pipeline->_camera.SetSpeed(_pipeline->_camera.movementSpeed + scrollY * speedSensitivity);
        //    }
        //}
        //else {
        //    glfwSetInputMode(window::_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        //    float dummyX, dummyY;
        //    input::getMouseDelta(dummyX, dummyY);
        //}

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
            ui::debug::drawDebugMenu(*_renderer->getPipeline());
        }

        ImGui::Render();
        _renderer->render(*world.get());
    }

    _renderer->waitDeviceIdle();
    //vkDeviceWaitIdle(_pipeline->getDevice());
}

int main() {
    logger::init();

    auto _core = std::make_unique<core>();
    auto _renderer = std::make_unique<renderer>();

    ui::debug::initialize(*_renderer); // initialize imgui debug ui

    auto engine = std::make_unique<Engine>(_core.get(), _renderer.get());
    Engine::_config.mainWindow = true;

    input::init(window::_window);

    _renderer->getPipeline()->_camera = Camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -90.0f,
        0.0f
    );

    engine->mainLoop();

    return 0;
}
