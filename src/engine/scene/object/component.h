#ifndef ENGINE_COMPONENT_H
#define ENGINE_COMPONENT_H

class Object;

// base component
class component {
public:
	component();
	virtual ~component() = default;

	void setOwner(Object* obj);

	Object* getOwner() const;

	virtual void addedToObject();

	virtual void update(float deltaTime);

	static const bool isUnique = false;
	bool canUpdate = true;

private:
	Object* owner = nullptr;
};

#endif // ENGINE_COMPONENT_H