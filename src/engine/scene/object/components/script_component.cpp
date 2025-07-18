#include "script_component.h"
#include "../object.h"
#include <logs.h>

ScriptComponent::ScriptComponent(std::unique_ptr<IScriptInstance> scriptInstance)
    : m_scriptInstance(std::move(scriptInstance))
{
    if (!m_scriptInstance) {
        LOG_ERROR("failed create script component with nullptr script instance");
    }
}

ScriptComponent::~ScriptComponent() {
    if (m_scriptInstance) {
        m_scriptInstance->OnDestroy();
        LOG_INFO("script (%s) was destroyed", GetScriptName().c_str());
    }
}

void ScriptComponent::addedToObject() {
    component::addedToObject();
    if (m_scriptInstance) {
        m_scriptInstance->SetOwner(getOwner());
        m_scriptInstance->OnCreate();
        LOG_INFO("script (%s) created and attached to object (ID %d).",
            GetScriptName().c_str(), getOwner()->getID());
    }
}

void ScriptComponent::update(float deltaTime) {
    if (m_scriptInstance && canUpdate) {
        m_scriptInstance->OnUpdate(deltaTime);
    }
}

const std::string& ScriptComponent::GetScriptName() const {
    if (m_scriptInstance) {
        return m_scriptInstance->GetScriptName();
    }

    static const std::string empty_string = "";
    return empty_string;
}