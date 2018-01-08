#include "AITDViewer.hpp"
#include <graphics/GraphicsEngine.hpp>

class AITDViewer : public GraphicsEngine {
public:
	AITDViewer(const char* name, const char* desc) : GraphicsEngine(name, desc) {
		//auto console = spd::stdout_logger_mt("Client");
	}

	void init_engine() override {
		engine.init();
	}

	void frame(const float dt) override {
		engine.run_frame(dt);
	}

private:

	AITDViewer engine;
	
};

ENTRY_IMPLEMENT_MAIN(AITD, "AITD ActorViewer", "Actor Viewer");

