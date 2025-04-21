#ifndef INPUT_INPUT_H
#define INPUT_INPUT_H

#include <unordered_map>
#include "utils/keycodes.h"

struct GLFWwindow;

class input {
public:
    static input& instance() {
        static input instance;
        return instance;
    }

    static void init(GLFWwindow* window);

    static void update();

    static bool IsKeyDown(keycode key);
    static bool WasKeyPressed(keycode key);
    static bool WasKeyReleased(keycode key);

    static void OnKeyEvent(int glfwKey, int action);

    static void OnCharEvent(unsigned int codepoint);

    static void addMouseDelta(float xoffset, float yoffset);
    static void getMouseDelta(float& xoffset, float& yoffset);

    static void addScrollDelta(float xoffset, float yoffset);
    static void getScrollDelta(float& xoffset, float& yoffset);

private:
    static std::unordered_map<keycode, bool> keyStates;
    static std::unordered_map<keycode, bool> keyDown;
    static std::unordered_map<keycode, bool> keyUp;

    static float mouseDeltaX;
    static float mouseDeltaY;
    static float scrollDeltaX;
    static float scrollDeltaY;
};

#endif // INPUT_INPUT_H
