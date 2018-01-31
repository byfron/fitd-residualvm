#include "AITDEngine.hpp"
#include "ObjectManager.hpp"
#include "ActorLoader.hpp"
#include "GameSystems.hpp"
#include "utils/Color.hpp"
#include "Components.hpp"
#include <graphics/RenderSystem.hpp>
#include <utils/DataParsing.hpp>
#include <main.h>
#include <resource.h>
#include <file_access.h>
#include "Message.hpp"

Entity::Id AITDEngine::player_entity_id;
std::map<uint32_t, int> AITDEngine::CameraEntityToIndex;
std::map<int, int16> AITDEngine::globals;
std::map<int, int16> AITDEngine::C_globals;

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
	// event_manager->subscribe<Msg::Move>(*update_sys);
	
	// Order matters!!
	add<InputSystem>(std::make_shared<InputSystem>());
	add<ScriptingSystem>(std::make_shared<ScriptingSystem>(scripting_manager));
	add<RenderSystem>(std::make_shared<RenderSystem>(world));
	add<CollisionSystem>(std::make_shared<CollisionSystem>());
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

	//load variables
	int16* var_buffer = (int16*)Fitd::g_resourceLoader->loadFromItd("VARS.ITD");
	int num_vars = Fitd::g_resourceLoader->getFileSize("VARS.ITD")/2;
	for (int i = 0; i < num_vars; i++) {
		globals[i] = var_buffer[i];
	}

	//load defines
	Common::SeekableReadStream *stream = Fitd::g_resourceLoader->getFile("DEFINES.ITD");
	if(!stream) {
		Fitd::theEnd(0, "DEFINES.ITD");
	}
	for (int i = 0; i < Fitd::g_fitd->getNumCVars(); i++) {
		C_globals[i] = stream->readUint16LE();
	}
	delete stream;

	for(int i = 0; i < Fitd::g_fitd->getNumCVars(); i++) {
	 	C_globals[i] = ((C_globals[i] & 0xFF) << 8) | ((C_globals[i] & 0xFF00) >> 8);
	}
	
	int actor_idx = 45;

	//create world
	world->load();
	
}

void AITDEngine::requestQuit() {
	running = false;
}
