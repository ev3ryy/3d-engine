#ifndef INPUT_INPUT_H
#define INPUT_INPUT_H

#include <unordered_map>
#include "utils/keycodes.h"

struct GLFWwindow;

class IInputProvider {
public:
    virtual ~IInputProvider() = default;

    virtual bool isKeyDown(keycode key) const = 0;
    virtual bool wasKeyPressed(keycode key) const = 0;
    virtual bool wasKeyReleased(keycode key) const = 0;

    virtual void getMouseDelta(float& xoffset, float& yoffset) const = 0;
    virtual void getScrollDelta(float& xoffset, float& yoffset) const = 0;
};

class Input : public IInputProvider {
public:
    static Input& instance() {
        static Input instance;
        return instance;
    }

    static void init(GLFWwindow* window);

    static void update();

    bool isKeyDown(keycode key) const override;
    bool wasKeyPressed(keycode key) const override;
    bool wasKeyReleased(keycode key) const override;

    void getMouseDelta(float& xoffset, float& yoffset) const override;
    void getScrollDelta(float& xoffset, float& yoffset) const override;

    static void onKeyEvent(int glfwKey, int action);
    static void onCharEvent(unsigned int codepoint);
    static void addMouseDelta(float xoffset, float yoffset);
    static void addScrollDelta(float xoffset, float yoffset);

private:
    Input() = default;
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    std::unordered_map<keycode, bool> keyStates;
    std::unordered_map<keycode, bool> keyDown;
    std::unordered_map<keycode, bool> keyUp;

    float mouseDeltaX;
    float mouseDeltaY;
    float scrollDeltaX;
    float scrollDeltaY;
};

#endif // INPUT_INPUT_H
