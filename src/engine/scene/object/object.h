#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <glm/glm.hpp>

#include "component.h"
#include "components/transform_component.h"
#include "components/mesh_renderer_component.h"

class World;

class Object {
public:
	Object(const std::string& name = "New Object");
	~Object();

	template<typename T, typename... Args>
	T* addComponent(Args&&... args) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from Component class");

		if constexpr (T::isUnique) {
			if (getComponent<T>() != nullptr) {
				return nullptr;
			}
		}

		auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);
		T* ptr = newComponent.get();
		ptr->setOwner(this);

		if constexpr (std::is_same_v<T, TransformComponent>) {
			transform = ptr;
		}

		components.push_back(std::move(newComponent));
		ptr->addedToObject();

		return ptr;
	}

	template<typename T>
	T* getComponent() const {
		static_assert(std::is_base_of<component, T>::value, "T must derive from Component class");

		if constexpr (std::is_same_v<T, TransformComponent>) {
			return transform;
		}

		for (const auto& comp_ptr : components) {
			if (T* specificComponent = dynamic_cast<T*>(comp_ptr.get())) {
				return specificComponent;
			}
		}

		return nullptr;
	}

	virtual void update(float deltaTime);

	const std::string& getName() const;
	int getID() const;

	glm::mat4 getLocalMatrix() const {
		if (transform) {
			return transform->getLocalMatrix();
		}

		return glm::mat4(1.0f);
	}

	glm::mat4 getWorldMatrix() const {
		if (transform) {
			return transform->getWorldMatrix();
		}

		return glm::mat4(1.0f);
	}

	TransformComponent* getTransform() const { return transform; }

	const std::vector<std::unique_ptr<Object>>& getChildren() const { return children; }
	Object* getParent() const { return parent; }
	World* getWorld() const { return m_world; }

	void setName(const std::string& newName);
	void setWorld(World* world);

	void addChild(std::unique_ptr<Object> child);
	void removeChild(Object* child);
	std::unique_ptr<Object> detachChild(Object* child);

	std::unique_ptr<Object> deepCopy() const;

	static int nextID;
	bool canUpdate;

private:
	std::string name;
	int id;

	World* m_world = nullptr;

	Object* parent = nullptr;
	std::vector<std::unique_ptr<Object>> children;

	std::vector<std::unique_ptr<component>> components;
	TransformComponent* transform = nullptr;
};

#endif // ENGINE_OBJECT_H