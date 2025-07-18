#include "input.h"
#include <logs.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    Input::onKeyEvent(key, action);
}

static void GLFWCharCallback(GLFWwindow* window, unsigned int codepoint) {
    ImGui_ImplGlfw_CharCallback(window, codepoint);
    Input::onCharEvent(codepoint);
}

static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

static void GLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;
    static float lastX = 0.0f;
    static float lastY = 0.0f;

    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }
    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos);

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    Input::addMouseDelta(xoffset, yoffset);
}

static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    Input::addScrollDelta(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

void Input::init(GLFWwindow* window) {
    if (!window) {
        LOG_ERROR("[input]: failed to find window");
        return;
    }

    Input::instance().keyStates.clear();
    Input::instance().keyDown.clear();
    Input::instance().keyUp.clear();
    Input::instance().mouseDeltaX = 0.0f;
    Input::instance().mouseDeltaY = 0.0f;
    Input::instance().scrollDeltaX = 0.0f;
    Input::instance().scrollDeltaY = 0.0f;

    glfwSetKeyCallback(window, GLFWKeyCallback);
    glfwSetCharCallback(window, GLFWCharCallback);
    glfwSetCursorPosCallback(window, GLFWCursorPosCallback);
    glfwSetScrollCallback(window, GLFWScrollCallback);
    glfwSetMouseButtonCallback(window, GLFWMouseButtonCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::update() {
    for (auto& state : instance().keyDown)
        state.second = false;
    for (auto& state : instance().keyUp)
        state.second = false;

    instance().mouseDeltaX = 0.0f;
    instance().mouseDeltaY = 0.0f;
    instance().scrollDeltaX = 0.0f;
    instance().scrollDeltaY = 0.0f;
}

bool Input::isKeyDown(keycode key) const {
    auto it = keyStates.find(key);
    return it != keyStates.end() ? it->second : false;
}
bool Input::wasKeyPressed(keycode key) const {
    auto it = keyDown.find(key);
    return it != keyDown.end() ? it->second : false;
}
bool Input::wasKeyReleased(keycode key) const {
    auto it = keyUp.find(key);
    return it != keyUp.end() ? it->second : false;
}

void Input::getMouseDelta(float& xoffset, float& yoffset) const {
    xoffset = mouseDeltaX;
    yoffset = mouseDeltaY;
}

void Input::getScrollDelta(float& xoffset, float& yoffset) const {
    xoffset = scrollDeltaX;
    yoffset = scrollDeltaY;
}

void Input::onKeyEvent(int glfwKey, int action) {
    keycode key = static_cast<keycode>(glfwKey);

    if (action == GLFW_PRESS) {
        instance().keyStates[key] = true;
        instance().keyDown[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        instance().keyStates[key] = false;
        instance().keyUp[key] = true;
    }
}

void Input::onCharEvent(unsigned int codepoint) {
    // ...
}

void Input::addMouseDelta(float xoffset, float yoffset) {
    instance().mouseDeltaX += xoffset;
    instance().mouseDeltaY += yoffset;
}

void Input::addScrollDelta(float xoffset, float yoffset) {
    instance().scrollDeltaX += xoffset;
    instance().scrollDeltaY += yoffset;
}