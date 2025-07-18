#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#include "../component.h"
#include "../script_instance.h"
#include <memory>
#include <string>

class ScriptComponent : public component {
public:
    static const bool isUnique = false;

    ScriptComponent(std::unique_ptr<IScriptInstance> scriptInstance);
    virtual ~ScriptComponent();

    virtual void addedToObject() override;
    virtual void update(float deltaTime) override;

    const std::string& GetScriptName() const;
    IScriptInstance* GetScriptInstance() const { return m_scriptInstance.get(); }

private:
    std::unique_ptr<IScriptInstance> m_scriptInstance;
};

#endif // SCRIPT_COMPONENT_H