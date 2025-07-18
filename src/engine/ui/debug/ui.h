#ifndef DEBUG_UI_H
#define DEBUG_UI_H

class renderer;
class World;

namespace ui {
	class debug {
	public:
		static void initialize(renderer& _renderer);
		static void drawDebugMenu(World& world, renderer& renderer);
	};
}

#endif // DEBUG_UI_H