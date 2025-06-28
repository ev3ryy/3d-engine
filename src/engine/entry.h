#ifndef ENGINE_ENTRY_H
#define ENGINE_ENTRY_H

#include <string>
#include <memory>
#include <chrono>
#include <functional>

class core;
class renderer;
class pipeline;
class camera;
class World;
class ResourceManager;
class IInputProvider;

struct GLFWwindow;

//struct config {
//    bool mainWindow;
//    std::string projectPath;
//};

class Engine {
public:
    Engine(core* coreInstance, renderer* rendererInstance);
    ~Engine();

    //void mainLoop();

    bool run(std::function<void(float deltaTime, World&, renderer&, ResourceManager&)> editorUpdateCallback,
        std::function<void(float deltaTime, World&, ResourceManager&, IInputProvider* inputProvider)> gameUpdateCallback,
        GLFWwindow* windowHandle);

    //static config _config;

    core* getCore() { return _core; }
    renderer* getRenderer() { return _renderer; }

private:
    core* _core;
    renderer* _renderer;

    std::unique_ptr<World> world;

    int last_fb_width = 0, last_fb_height = 0;

    float getDeltaTime();
    std::chrono::high_resolution_clock::time_point lastTime;
};

#endif // ENGINE_ENTRY_H