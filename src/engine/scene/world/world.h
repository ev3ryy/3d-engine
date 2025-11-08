#ifndef SCENE_WORLD_H
#define SCENE_WORLD_H

#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <type_traits>

class MeshRendererComponent;
class Object;
class Camera;

class World {
public:
	World();
	~World();

	World(const World&) = delete;
	World& operator=(const World&) = delete;

	World(World&&) = default;
	World& operator=(World&&) = default;

	void addObject(std::unique_ptr<Object> obj);
	void removeRootObject(Object* obj);

	template<typename T = Object, typename... Args>
	T* createObject(const std::string& name = "New Object", Args&&... args) {
		static_assert(std::is_base_of<Object, T>::value, "T must derive from Object");
		std::unique_ptr<T> newObj = std::make_unique<T>(name, std::forward<Args>(args)...);
		T* ptr = newObj.get();
		ptr->setWorld(this);

		addObject(std::move(newObj));
		return ptr;
	}

	const std::vector<std::unique_ptr<Object>>& getAllObjects() const;
	std::vector<Object*> getAllRawObjects() const;

	Object* findObjectByID(int id);
	Object* findObjectByName(const std::string& name);
	const Camera& getActiveRenderCamera() const;

	void setActiveRenderCamera(const Camera* cam);

	const glm::vec3& getSunDirection() const { return m_sunDirection; }
	void setSunDirection(const glm::vec3& dir) { m_sunDirection = dir; }

	float getSunIntensity() const { return m_sunIntensity; }
	void setSunIntensity(float intensity) { m_sunIntensity = intensity; }

	//std::vector<Object*> getRenderableObjects() const;

	void update(float deltaTime);
	void clear();

private:
	glm::vec3 m_sunDirection{ -1.0f, -1.0f, -0.5f };
	float m_sunIntensity = 100.0f;

	std::vector<std::unique_ptr<Object>> objects;

	// fast access
	std::unordered_map<int, Object*> objectIdMap;
	std::unordered_map<std::string, Object*> objectNameMap;

	const Camera* activeRenderCamera = nullptr;

	void addObjectsToMapsRecursive(Object* obj);
	void removeObjectsFromMapsRecursive(Object* obj);
	void updateObjectRecursive(Object* obj, float deltaTime);
};

#endif // SCENE_WORLD_H