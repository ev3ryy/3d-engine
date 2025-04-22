#ifndef ENGINE_COMPONENT_H
#define ENGINE_COMPONENT_H

class object;

// base component
class component {
public:
	virtual ~component() = default;

	void setOwner(object* obj);

	object* getOwner() const;

	virtual void addedToObject();

	virtual void update(float deltaTime);

	static const bool isUnique = false;

private:
	object* owner = nullptr;
};

#endif // ENGINE_COMPONENT_H