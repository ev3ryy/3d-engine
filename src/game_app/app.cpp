#include "app.h"

#include <core.h>
#include <renderer.h>
#include <input.h>
#include <logs.h>
#include <entry.h>
#include <resources/manager/resource_manager.h>
#include <GLFW/glfw3.h>

#include <window/window.h>

#ifdef _WIN32
#include <Windows.h>
#define LOAD_DLL_LIB LoadLibraryA
#define FREE_DLL_LIB FreeLibrary
#define GET_FUNC_ADDR GetProcAddress
#else
#include <dlfcn.h>
#define LOAD_DLL_LIB(path) dlopen(path, RTLD_LAZY | RTLD_GLOBAL)
#define FREE_DLL_LIB dlclose
#define GET_FUNC_ADDR dlsym
#endif

GameApplication::GameApplication() {
}

GameApplication::~GameApplication() {
    unloadGameDll();
}

bool GameApplication::initialize() {
    logger::init();
    LOG_INFO("GameApp: Logger initialized.");

    if (!glfwInit()) {
        LOG_ERROR("GameApp: Failed to initialize GLFW.");
        return false;
    }
    LOG_INFO("GameApp: GLFW initialized.");

    m_core = std::make_unique<core>();
    m_renderer = std::make_unique<renderer>();

    LOG_INFO("GameApp: Main window handle obtained from renderer.");

    Input::init(m_mainWindowHandle);
    LOG_INFO("GameApp: Input initialized.");

    m_engine = std::make_unique<Engine>(m_core.get(), m_renderer.get());
    LOG_INFO("GameApp: EngineRuntime initialized.");

    if (!loadGameDll("Game.dll")) {
        LOG_ERROR("GameApp: Failed to load Game.dll. Game application cannot proceed.");
        return false;
    }
    LOG_INFO("GameApp: Game.dll successfully loaded and callback obtained.");

    return true;
}

int GameApplication::run() {
    if (!m_engine) {
        LOG_ERROR("GameApp: Engine is not initialized. Cannot run game loop.");
        return 1;
    }

    auto editorUpdateCallback = [&](float deltaTime, World& world, renderer& rendererInstance, ResourceManager& resourceManager) {

    };

    auto gameLogicUpdateCallback = [&](float deltaTime, World& world, ResourceManager& resourceManager, IInputProvider* inputProvider) {
        if (m_gameUpdateFunc) {
            m_gameUpdateFunc(deltaTime, world, ResourceManager::Get(), inputProvider);
        }
    };

    LOG_INFO("GameApp: Starting main game loop.");
    try {
        m_engine->run(editorUpdateCallback, gameLogicUpdateCallback, m_mainWindowHandle);
    }
    catch (const std::exception& e) {
        LOG_ERROR("GameApp: Engine runtime error during game loop: %s", e.what());
        return 1;
    }

    LOG_INFO("GameApp: Main game loop finished.");
    return 0;
}

bool GameApplication::loadGameDll(const char* dllPath) {
    m_gameDllHandle = LOAD_DLL_LIB(dllPath);
    if (m_gameDllHandle) {
        RawGameUpdateCallback(*getGameUpdateFuncPtr)() =
            (RawGameUpdateCallback(*)())GET_FUNC_ADDR(
#ifdef _WIN32
            (HMODULE)m_gameDllHandle,
#else
                m_gameDllHandle,
#endif
                "GetGameUpdateCallback"
            );

        if (getGameUpdateFuncPtr) {
            m_gameUpdateFunc = getGameUpdateFuncPtr();
            if (m_gameUpdateFunc) {
                LOG_INFO("GameApp: 'GetGameUpdateCallback' successfully obtained from Game.dll.");
                return true;
            }
            else {
                LOG_ERROR("GameApp: 'GetGameUpdateCallback' returned null from Game.dll.");
            }
        }
        else {
            LOG_ERROR("GameApp: Failed to get 'GetGameUpdateCallback' address from Game.dll (symbol not found).");
        }
    }
    else {
        std::string error_msg;
#ifdef _WIN32
        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
        if (messageBuffer) {
            error_msg = std::string(messageBuffer, size);
            LocalFree(messageBuffer);
        }
        else {
            error_msg = "Unknown Windows error.";
        }
#else
        error_msg = dlerror();
#endif
        LOG_ERROR("GameApp: Failed to load Game.dll. Error: %s", error_msg.c_str());
    }
    return false;
}

void GameApplication::unloadGameDll() {
    if (m_gameDllHandle) {
        FREE_DLL_LIB(
#ifdef _WIN32
        (HMODULE)m_gameDllHandle
#else
            m_gameDllHandle
#endif
        );
        m_gameDllHandle = nullptr;
        m_gameUpdateFunc = nullptr;
        LOG_INFO("GameApp: Game.dll unloaded.");
    }
}

int main() {
    GameApplication app;
    if (!app.initialize()) {
        LOG_ERROR("GameApp: Application initialization failed. Exiting.");
        return 1;
    }
    return app.run();
}