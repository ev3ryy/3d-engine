#ifndef DEBUG_UI_H
#define DEBUG_UI_H

class renderer;
class pipeline;

namespace ui {
	class debug {
	public:
		static void initialize(renderer& _renderer);
		static void drawDebugMenu(pipeline& _pipeline);
	};
}

#endif // DEBUG_UI_H