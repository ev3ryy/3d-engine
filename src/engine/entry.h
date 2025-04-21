#ifndef ENGINE_ENTRY_H
#define ENGINE_ENTRY_H

class core;
class renderer;
class pipeline;
class camera;

#include <string>

struct config {
    bool mainWindow;
    std::string projectPath;
};

class entry {
public:
    entry(core* coreInstance, renderer* rendererInstance);
    ~entry();

    void mainLoop();

    static config _config;

private:
    core* _core;
    renderer* _renderer;
    pipeline* _pipeline;

    int last_fb_width = 0, last_fb_height = 0;
};

#endif // ENGINE_ENTRY_H