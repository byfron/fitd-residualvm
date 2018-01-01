#include "AITDEngine.hpp"
#include "ActorFactory.hpp"
#include "utils/Color.hpp"
#include <graphics/RenderSystem.hpp>

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

	// Create system manager
	entity_manager = EntityManager::Ptr(new EntityManager());
	event_manager = EventManager::Ptr(new EventManager());
	system_manager = SystemManager::Ptr(new SystemManager(*entity_manager, *event_manager));

	// Add Render system
	add<RenderSystem>(std::make_shared<RenderSystem>());
	
}

void AITDEngine::loadGameData() {

	ColorPalette::loadPalette();

	int actor_idx = 45;
	//create entity
	Actor::Ptr actor_data = ActorFactory::create(actor_idx);
	
	Entity actor = entity_manager->createLocal();
	entity_manager->assign<MeshComponent>(actor.id(), actor_data->getMesh());
	
}

void AITDEngine::requestQuit() {
	running = false;
}
