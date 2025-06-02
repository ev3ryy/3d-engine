#ifndef ENGINE_ENTRY_H
#define ENGINE_ENTRY_H

class core;
class renderer;
class pipeline;
class camera;
class World;

#include <string>
#include <memory>

struct config {
    bool mainWindow;
    std::string projectPath;
};

class Engine {
public:
    Engine(core* coreInstance, renderer* rendererInstance);
    ~Engine();

    void mainLoop();

    static config _config;

private:
    core* _core;
    renderer* _renderer;

    std::unique_ptr<World> world;

    int last_fb_width = 0, last_fb_height = 0;
};

#endif // ENGINE_ENTRY_H