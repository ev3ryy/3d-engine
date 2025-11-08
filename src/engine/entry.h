#ifndef ENGINE_ENTRY_H
#define ENGINE_ENTRY_H

#include <string>
#include <memory>
#include <chrono>
#include <functional>
#include <future>

class core;
class renderer;
class pipeline;
class camera;
class World;
class ResourceManager;
class IInputProvider;
class PhysicsWorld;

struct GLFWwindow;

enum class EngineState {
    EDITING,
    PLAYING
};

class Engine {
public:
    Engine(core* coreInstance, renderer* rendererInstance);
    ~Engine();

    bool run(std::function<void(World&, renderer&)> editorUpdateCallback,
        std::function<void(float deltaTime, World&, ResourceManager&, IInputProvider* inputProvider)> gameUpdateCallback,
        GLFWwindow* windowHandle);

    core* getCore() { return _core; }
    renderer* getRenderer() { return _renderer; }

    EngineState getCurrentState() const { return currentState; }
    void setState(EngineState newState) {
        if (currentState != newState) {
            currentState = newState;
            onStateChanged(newState);
        }
    }

private:
    core* _core;
    renderer* _renderer;

    std::unique_ptr<PhysicsWorld> physicsWorld;
    std::unique_ptr<World> world;

    std::future<void> physicsFuture;

    int last_fb_width = 0, last_fb_height = 0;

    float getDeltaTime();
    std::chrono::high_resolution_clock::time_point lastTime;

    void onStateChanged(EngineState newState);

    EngineState currentState = EngineState::EDITING;
};

#endif // ENGINE_ENTRY_H