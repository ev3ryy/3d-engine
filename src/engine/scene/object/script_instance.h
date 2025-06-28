#ifndef SCRIPT_INSTANCE_H
#define SCRIPT_INSTANCE_H

#include <string>
#include <memory>

class Object;

class IScriptInstance {
public:
    virtual ~IScriptInstance() = default;

    virtual void OnCreate() = 0;
    virtual void OnUpdate(float deltaTime) = 0;
    virtual void OnDestroy() = 0;

    virtual void SetOwner(Object* owner) = 0;
    virtual Object* GetOwner() const = 0;

    virtual const std::string& GetScriptName() const = 0;
};

#endif // SCRIPT_INSTANCE_H