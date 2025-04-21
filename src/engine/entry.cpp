#include "entry.h"

#include "ui/debug/ui.h"

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

config entry::_config;

entry::entry(core* coreInstance, renderer* rendererInstance)
    : _core(coreInstance), _renderer(rendererInstance)
{
    if (!_renderer) {
        throw std::runtime_error("Renderer is null");
    }

    _pipeline = _renderer->getPipeline();

    if (!_pipeline) {
        LOG_CRITICAL("pipeline pointer is null");
    }
}

entry::~entry() {

}

void entry::mainLoop() {
    bool flag = false;;

    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    _pipeline->imClearColor = clearColor;

    while (!glfwWindowShouldClose(window::_window)) {
        static auto lastTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        input::update();
        glfwPollEvents();

        bool cameraControlActive = (glfwGetMouseButton(window::_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

        if (cameraControlActive) {
            glfwSetInputMode(window::_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            bool shiftDown = input::IsKeyDown(keycode::LShift);

            float speedMultiplier = shiftDown ? 2.0f : 1.0f;
            float adjustedDeltaTime = deltaTime * speedMultiplier;

            if (input::IsKeyDown(keycode::W))
                _pipeline->_camera.ProcessKeyboard(Camera_Movement::FORWARD, adjustedDeltaTime);
            if (input::IsKeyDown(keycode::S))
                _pipeline->_camera.ProcessKeyboard(Camera_Movement::BACKWARD, adjustedDeltaTime);
            if (input::IsKeyDown(keycode::A))
                _pipeline->_camera.ProcessKeyboard(Camera_Movement::LEFT, adjustedDeltaTime);
            if (input::IsKeyDown(keycode::D))
                _pipeline->_camera.ProcessKeyboard(Camera_Movement::RIGHT, adjustedDeltaTime);

            float xoffset, yoffset;
            input::getMouseDelta(xoffset, yoffset);
            if (xoffset != 0.0f || yoffset != 0.0f)
                _pipeline->_camera.ProcessMouseMovement(xoffset, yoffset);

            float scrollX, scrollY;
            input::getScrollDelta(scrollX, scrollY);
            if (scrollY != 0.0f) {
                const float speedSensitivity = 0.1f; // Коэффициент изменения скорости
                _pipeline->_camera.SetSpeed(_pipeline->_camera.movementSpeed + scrollY * speedSensitivity);
            }
        }
        else {
            glfwSetInputMode(window::_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            float dummyX, dummyY;
            input::getMouseDelta(dummyX, dummyY);
        }

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
            ui::debug::drawDebugMenu(*_pipeline);
        }

        ImGui::Render();
        _pipeline->drawFrame();
    }

    vkDeviceWaitIdle(_pipeline->getDevice());
}

int main() {
    logger::init();

    auto _core = std::make_unique<core>();
    auto _renderer = std::make_unique<renderer>();

    ui::debug::initialize(*_renderer); // initialize imgui debug ui

    auto _entry = std::make_unique<entry>(_core.get(), _renderer.get());
    entry::_config.mainWindow = true;

    input::init(window::_window);

    _renderer->getPipeline()->_camera = camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -90.0f,
        0.0f
    );

    _entry->mainLoop();

    return 0;
}
