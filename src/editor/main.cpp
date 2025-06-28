#include <iostream>
#include <memory>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <vulkan/imgui_impl_glfw.h>
#include <vulkan/imgui_impl_vulkan.h>

#include <core.h>
#include <renderer.h>
#include <input.h>
#include <logger.h>
#include <logs.h>
#include <entry.h>
#include <resources/manager/resource_manager.h>
#include "ui/debug/ui.h"

#include <scene/object/components/script_component.h>

#include <window/window.h>

// game dll loading enable
#ifdef _WIN32
#include <Windows.h>
#define LOAD_DLL(path) LoadLibraryA(path)
#define FREE_DLL FreeLibrary
#define GET_FUNC GetProcAddress
#else
#include <dlfcn.h>
#define LOAD_DLL(path) dlopen(path, RTLD_LAZY | RTLD_GLOBAL)
#define FREE_DLL dlclose
#define GET_FUNC dlsym
#endif

//typedef void(*RawGameUpdateCallback)(float deltaTime, World& world, ResourceManager& resourceManager, IInputProvider* inputProvider);
//typedef RawGameUpdateCallback(*GetGameUpdateFuncPtr)();
//
//class IScriptInstance;
//struct ScriptInfo {
//    std::string scriptName;
//    std::unique_ptr<IScriptInstance>(*createFunc)();
//};
//using GetScriptRegistryFunc = const std::vector<ScriptInfo>& (*)();

int main() {
	logger::init();

	auto _core = std::make_unique<core>();
	auto _renderer = std::make_unique<renderer>();

	ui::debug::initialize(*_renderer.get());

	auto engine = std::make_unique<Engine>(_core.get(), _renderer.get());

	Input::init(window::_window);

    void* gameDllHandle = nullptr;
    const char* gameDllPath = "Game.dll";

    gameDllHandle = LOAD_DLL(gameDllPath);
    if (gameDllHandle) {
        LOG_INFO("Game.dll loaded");
        ResourceManager::Get().LoadAndRegisterScriptFactories(gameDllHandle);
    }
    else {
        LOG_ERROR("failed to load Game.dll.");
    }

    auto editorUpdateCallback = [&](float deltaTime, World& world, renderer& rendererInstance, ResourceManager& resourceManager) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        PerformanceStats stats = window::updatePerfomanceStats();

        ui::debug::drawDebugMenu(world, rendererInstance);

        ImGui::Render();
    };

    //auto gameLogicUpdateCallback = [&](float deltaTime, World& world, ResourceManager& resourceManager, IInputProvider* inputProvider) {
    //    if (gameUpdateFunc) {
    //        gameUpdateFunc(deltaTime, world, ResourceManager::Get(), inputProvider);
    //    }
    //};

    try {
        engine->run(editorUpdateCallback,
            nullptr,
            window::_window
        );
    }
    catch (const std::exception& e) {
        LOG_ERROR("Engine runtime error: %s", e.what());
    }

    if (gameDllHandle) {
#ifdef _WIN32
        FreeLibrary((HMODULE)gameDllHandle);
#else
        dlclose(gameDllHandle);
#endif
        LOG_INFO("Game.dll unloaded.");
    }

    return 0;
}