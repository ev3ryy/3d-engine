#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

#include <scene/object/script_instance.h> 

#include <logs.h>

#ifdef _WIN32
#ifdef GAME_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif
#else
#define GAME_API __attribute__((visibility("default")))
#endif

class Object;
class transformComponent;

class GAME_API ScriptBase : public IScriptInstance {
public:
    ScriptBase(std::string scriptName);
    virtual ~ScriptBase() = default;

    virtual void OnCreate() override {}
    virtual void OnUpdate(float deltaTime) override {}
    virtual void OnDestroy() override {}

    virtual void SetOwner(Object* owner) override { m_owner = owner; }
    virtual Object* GetOwner() const override { return m_owner; }
    virtual const std::string& GetScriptName() const override { return m_scriptName; }

    template<typename T>
    T* GetComponent() const;

protected:
    Object* m_owner;
    const std::string m_scriptName;
};

#include <scene/object/object.h>
template<typename T>
T* ScriptBase::GetComponent() const {
    if (m_owner) {
        return m_owner->getComponent<T>();
    }
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::unique_ptr<IScriptInstance>(*ScriptFactoryFunc)();

struct GAME_API ScriptInfo {
    std::string scriptName;
    ScriptFactoryFunc createFunc;
};

extern "C" GAME_API const std::vector<ScriptInfo>& GetScriptRegistry();

extern std::vector<ScriptInfo> s_scriptRegistry;

namespace detail {
    template<typename T>
    class ScriptRegister {
    public:
        ScriptRegister(const std::string& scriptName) {
            static_assert(std::is_base_of<ScriptBase, T>::value, "Registered script class must inherit from GameScriptBase");

            s_scriptRegistry.push_back({
                scriptName,
                []() -> std::unique_ptr<IScriptInstance> {
                    return std::make_unique<T>();
                }
                });
            LOG_INFO("Game.dll: registered script: %s", scriptName.c_str());
        }
    };
}

#define REGISTER_SCRIPT(ClassName) \
    static detail::ScriptRegister<ClassName> s_Register_##ClassName(#ClassName)