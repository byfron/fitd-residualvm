#include "AITDEngine.hpp"
#include "ActorFactory.hpp"
#include "utils/Color.hpp"
//#include "FloorFactory.hpp"
#include <main.h>

void AITDEngine::init() {

	Fitd::init();
	createSubsystems();
	loadGameData();
}

AITDEngine::~AITDEngine() {
	ActorFactory::cleanBuffers();
}

void AITDEngine::createSubsystems() {

//	add<RenderSystem>(std::make_shared<RenderSystem>(_world, _visManager));

}

void AITDEngine::loadGameData() {

	ColorPalette::loadPalette();

	
	//TODO
//	floor_map[0] = FloorFactory::create(0);
	
	int actor_idx = 45;
	actor_map[actor_idx] = ActorFactory::create(actor_idx);

}

void AITDEngine::requestQuit() {
	running = false;
}
