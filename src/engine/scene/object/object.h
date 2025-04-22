#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include <string>
#include <vector>
#include <memory>

#include "component.h"
#include "components/transform_component.h"
#include <logger.h>

class object {
public:
	object(const std::string& name = "New Object");
	~object();

	template<typename T, typename... Args>
	T* addComponent(Args&&... args) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component class");

		if constexpr (T::isUnique) {
			if (getComponent<T>() != nullptr) {
				LOG_WARN("Object '%s' already has a unique component", name);
				return nullptr;
			}
		}

		auto component = std::make_unique<T>(std::forward<Args>(args)...);
		T* ptr = component.get();
		component->setOwner(this);

		if constexpr (std::is_same_v<T, transformComponent>) {
			transform = ptr;
		}

		components.push_back(std::move(component));
		ptr->addedToObject();

		return ptr;
	}

	template<typename T>
	T* getComponent() const {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component class");

		if constexpr (std::is_same_v<T, transformComponent>) {
			return transform;
		}

		for (const auto& component : components) {
			if (T* specificComponent = dynamic_cast<T*>(component.get())) {
				return specificComponent;
			}
		}

		return nullptr;
	}

	void update(float deltaTime);
	
	const std::string& getName() const;
	int getID() const;

	glm::mat4 getLocalMatrix() const {
		if (transform) {
			return transform->getWorldMatrix(); // @FIXME: getWorldMatrix = getLocalMatrix
		}

		return glm::mat4(1.0f);
	}

	static int nextID;

private:
	std::string name;
	int id;

	object* parent = nullptr;
	std::vector<object*> children;

	std::vector<std::unique_ptr<component>> components;
	transformComponent* transform = nullptr;
};

#endif // ENGINE_OBJECT_H
