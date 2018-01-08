#include <tools/AITDViewer.hpp>
#include <graphics/GraphicsEngine.hpp>

class ViewerGUI : public GraphicsEngine {
public:
	ViewerGUI(const char* name, const char* desc) : GraphicsEngine(name, desc) {
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

ENTRY_IMPLEMENT_MAIN(ViewerGUI, "AITD ActorViewer", "Actor Viewer");

