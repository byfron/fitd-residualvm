#include "AITDEngine.hpp"
#include "ObjectManager.hpp"
#include "ActorLoader.hpp"
#include "GameSystems.hpp"
#include "utils/Color.hpp"
#include "Components.hpp"
#include <graphics/RenderSystem.hpp>
#include <utils/DataParsing.hpp>
#include <main.h>
#include "Message.hpp"

Entity::Id AITDEngine::player_entity;

AITDEngine::AITDEngine() {	
}

AITDEngine::~AITDEngine() {
	ActorLoader::cleanBuffers();
}

void AITDEngine::init() {

	Fitd::init(false);
	DataParsing::init();
	
	createSubsystems();
	loadGameData();

	// Add systems
	std::shared_ptr<UpdateSystem> update_sys = std::make_shared<UpdateSystem>(world);

	// Suscribe for events
//	event_manager->subscribe<Msg::Move>(*update_sys);

	// Order matters!!
	add<InputSystem>(std::make_shared<InputSystem>());
	add<RenderSystem>(std::make_shared<RenderSystem>(world));
	add<UpdateSystem>(update_sys);

}

void AITDEngine::createSubsystems() {

	// Create system manager
	entity_manager = EntityManager::Ptr(new EntityManager());
	event_manager = EventManager::Ptr(new EventManager());
	system_manager = SystemManager::Ptr(new SystemManager(*entity_manager, *event_manager));
	world = World::Ptr(new World(entity_manager));
	scripting_manager = ScriptingManager::Ptr(new ScriptingManager(entity_manager, world));	
}

void AITDEngine::loadGameData() {

	ColorPalette::loadPalette();
	ObjectManager::loadObjects();

	scripting_manager->loadScriptsFromObjects(ObjectManager::object_map);
	
	int actor_idx = 45;

	//create world
	world->load();
	
}

void AITDEngine::requestQuit() {
	running = false;
}
