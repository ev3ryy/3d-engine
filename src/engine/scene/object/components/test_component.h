#ifndef TEST_COMPONENT_H
#define TEST_COMPONENT_H

#include "../component.h"

class testComponent : public component {
public:
	testComponent();

	virtual void update(float deltaTime) override;
};

#endif // TEST_COMPONENT_H