#ifndef TEST_OBJECT_H
#define TEST_OBJECT_H

#include "object.h"

class testComponent;

class testObject : public Object {
public:
	testObject(const std::string& name = "New Object");
	~testObject();

	testComponent* simpleComponent;

	void update(float deltaTime) override;
};

#endif // TEST_OBJECT_H