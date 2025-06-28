#include "core.h"

#include <logs.h>

core::core()
{
	initializeModule();
}

core::~core()
{
	LOG_INFO("Core module: shutdown");
}

int core::initializeModule()
{
	LOG_INFO("Core module: initialized!");
	return 0;
}
