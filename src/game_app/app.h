#ifndef GAME_APP_H
#define GAME_APP_H;

#include <memory>
#include <functional>

class core;
class renderer;
class Engine;
class World;
class ResourceManager;
struct GLFWwindow;

//typedef void(*RawGameUpdateCallback)(float deltaTime, World& world, ResourceManager& resourceManager, IInputProvider* inputProvider);
//typedef RawGameUpdateCallback(*GetGameUpdateFuncPtr)();

class GameApplication {
public:
    GameApplication();
    ~GameApplication();

    bool initialize();

    int run();

private:
    std::unique_ptr<core> m_core;
    std::unique_ptr<renderer> m_renderer;
    std::unique_ptr<Engine> m_engine;

    GLFWwindow* m_mainWindowHandle = nullptr;

    void* m_gameDllHandle = nullptr;
    //RawGameUpdateCallback m_gameUpdateFunc = nullptr;

    bool loadGameDll(const char* dllPath);

    void unloadGameDll();
};

#endif // GAME_APP_H