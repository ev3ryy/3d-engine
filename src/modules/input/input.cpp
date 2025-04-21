#include "input.h"
#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

std::unordered_map<keycode, bool> input::keyStates;
std::unordered_map<keycode, bool> input::keyDown;
std::unordered_map<keycode, bool> input::keyUp;
float input::mouseDeltaX = 0.0f;
float input::mouseDeltaY = 0.0f;
float input::scrollDeltaX = 0.0f;
float input::scrollDeltaY = 0.0f;

static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    input::OnKeyEvent(key, action);
}

static void GLFWCharCallback(GLFWwindow* window, unsigned int codepoint) {
    ImGui_ImplGlfw_CharCallback(window, codepoint);
    input::OnCharEvent(codepoint);
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
    float yoffset = lastY - static_cast<float>(ypos); // инвертируем Y-ось

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    input::addMouseDelta(xoffset, yoffset);
}

static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    input::addScrollDelta(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

void input::init(GLFWwindow* window) {
    if (!window) {
        LOG_ERROR("[input]: failed to find window");
        return;
    }

    glfwSetKeyCallback(window, GLFWKeyCallback);
    glfwSetCharCallback(window, GLFWCharCallback);
    glfwSetCursorPosCallback(window, GLFWCursorPosCallback);
    glfwSetScrollCallback(window, GLFWScrollCallback);
    glfwSetMouseButtonCallback(window, GLFWMouseButtonCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void input::update() {
    for (auto& state : keyDown)
        state.second = false;
    for (auto& state : keyUp)
        state.second = false;
}

bool input::IsKeyDown(keycode key) {
    auto it = keyStates.find(key);
    return it != keyStates.end() ? it->second : false;
}
bool input::WasKeyPressed(keycode key) {
    auto it = keyDown.find(key);
    return it != keyDown.end() ? it->second : false;
}
bool input::WasKeyReleased(keycode key) {
    auto it = keyUp.find(key);
    return it != keyUp.end() ? it->second : false;
}

void input::OnKeyEvent(int glfwKey, int action) {
    keycode key = static_cast<keycode>(glfwKey);
    if (action == GLFW_PRESS) {
        keyStates[key] = true;
        keyDown[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        keyStates[key] = false;
        keyUp[key] = true;
    }
}

void input::OnCharEvent(unsigned int /*codepoint*/) {

}

void input::addMouseDelta(float xoffset, float yoffset) {
    mouseDeltaX += xoffset;
    mouseDeltaY += yoffset;
}

void input::getMouseDelta(float& xoffset, float& yoffset) {
    xoffset = mouseDeltaX;
    yoffset = mouseDeltaY;

    mouseDeltaX = 0.0f;
    mouseDeltaY = 0.0f;
}

void input::addScrollDelta(float xoffset, float yoffset) {
    scrollDeltaX += xoffset;
    scrollDeltaY += yoffset;
}

void input::getScrollDelta(float& xoffset, float& yoffset) {
    xoffset = scrollDeltaX;
    yoffset = scrollDeltaY;
    scrollDeltaX = 0.0f;
    scrollDeltaY = 0.0f;
}
