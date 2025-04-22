#ifndef SCENE_WORLD_H
#define SCENE_WORLD_H

#include <vector>

#include "../object/object.h"

class world {
public:
	world();
	~world() = default;

	void addObject(std::unique_ptr<object> obj);

	template<typename T = object, typename... Args>
	T* createObject(const std::string& name = "New Object", Args&&... args) {
		static_assert(std::is_base_of<Object, T>::value, "T must derive from object");
		std::unique_ptr<T> newObj = std::make_unique<T>(name, std::forward<Args>(args)...);
		T* ptr = newObj.get();
		objects.push_back(newObj);
		return ptr;
	}

	const std::vector<std::unique_ptr<object>>& getAllObjects() const;

	void update(float deltaTime);
	void clear();

private:
	std::vector<std::unique_ptr<object>> objects;
};

#endif // SCENE_WORLD_H