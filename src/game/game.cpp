#include "game.h"
#include <vector>
#include <memory>

std::vector<ScriptInfo> s_scriptRegistry;

ScriptBase::ScriptBase(std::string scriptName)
    : m_owner(nullptr), m_scriptName(std::move(scriptName)) {
}

extern "C" GAME_API const std::vector<ScriptInfo>& GetScriptRegistry() {
    return s_scriptRegistry;
}