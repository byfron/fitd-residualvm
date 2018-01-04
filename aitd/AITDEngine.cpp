#include "AITDEngine.hpp"
#include "ActorLoader.hpp"
#include "utils/Color.hpp"
#include <graphics/RenderSystem.hpp>

//#include "FloorFactory.hpp"
#include <main.h>

AITDEngine::AITDEngine() {
	world = World::Ptr(new World());
}

AITDEngine::~AITDEngine() {
	ActorLoader::cleanBuffers();
}


void AITDEngine::init() {

	Fitd::init();	
	createSubsystems();
	loadGameData();
}

void AITDEngine::createSubsystems() {

	// Create system manager
	entity_manager = EntityManager::Ptr(new EntityManager());
	event_manager = EventManager::Ptr(new EventManager());
	system_manager = SystemManager::Ptr(new SystemManager(*entity_manager, *event_manager));

	// Add Render system
	add<RenderSystem>(std::make_shared<RenderSystem>(world));
	
}

void AITDEngine::loadGameData() {

	ColorPalette::loadPalette();

	int actor_idx = 45;
	
	//create actor entity
	Actor::Ptr actor_data = ActorLoader::load(actor_idx);	
	Entity actor = entity_manager->createLocal();
	entity_manager->assign<MeshComponent>(actor.id(), actor_data->getMesh());

	//create world
	world->load();
	
}

void AITDEngine::requestQuit() {
	running = false;
}
