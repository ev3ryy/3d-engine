#include "entry.h"

#include "ui/debug/ui.h"
#include "scene/world/world.h"
#include "scene/object/components/camera_component.h"

#include "resources/manager/resource_manager.h"

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

#include <physics/physics.h>

#include <iostream>
#include <filesystem>
#include <sstream>

Engine::Engine(core* coreInstance, renderer* rendererInstance)
    : _core(coreInstance), _renderer(rendererInstance)
{
    if (!_renderer) {
        throw std::runtime_error("Renderer is null");
    }

    ResourceManager::Get().Initialize(_renderer);

    lastTime = std::chrono::high_resolution_clock::now();
}

Engine::~Engine() {
    _renderer->waitDeviceIdle();
}

float Engine::getDeltaTime() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;
    return deltaTime;
}

bool Engine::run(std::function<void(float deltaTime, World&, renderer&, ResourceManager&)> editorUpdateCallback,
    std::function<void(float deltaTime, World&, ResourceManager&, IInputProvider* inputProvider)> gameUpdateCallback,
    GLFWwindow* windowHandle)
{
    m_physicsFacade = std::make_unique<Physics>();
    world = std::make_unique<World>(m_physicsFacade.get());

    auto cameraObj = world->createObject("MainCamera");
    auto cameraComp = cameraObj->addComponent<CameraComponent>(
        glm::vec3(0.0f, 0.0f, 3.0f), // position
        glm::vec3(0.0f, 1.0f, 0.0f), // up
        -90.0f,                      // yaw
        0.0f                         // pitch
    );

    const float fixedTimeStep = 1.0f / 60.0f;
    float accumulator = 0.0f;

    world->setActiveRenderCamera(&cameraComp->camera);

    while (!glfwWindowShouldClose(window::_window)) {
        float deltaTime = getDeltaTime();
        accumulator += deltaTime;

        Input::update();
        glfwPollEvents();

        while (accumulator >= fixedTimeStep) {
            m_physicsFacade->update(fixedTimeStep);

            // world->fixedUpdate(fixedTimeStep); 

            accumulator -= fixedTimeStep;
        }

        world->update(deltaTime);

        if (gameUpdateCallback) {
            gameUpdateCallback(deltaTime, *world.get(), ResourceManager::Get(), &Input::instance());
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

        if (editorUpdateCallback) {
            editorUpdateCallback(deltaTime, *world.get(), *_renderer, ResourceManager::Get());
        }

        _renderer->render(*world.get(), ResourceManager::Get());
    }

    return false;
}
