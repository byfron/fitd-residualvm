#pragma once

#include "World.hpp"
#include "ScriptingManager.hpp"
#include "entities/Entity.hpp"
#include "entities/System.hpp"
#include <map>

class AITDEngine {
public:
	
	AITDEngine();
	~AITDEngine();
	void init();
	void createSubsystems();
	void loadGameData();
	void requestQuit();
	
	template <typename S>
	void add(std::shared_ptr<S> system) {
		system_manager->add<S>(system);
	}
	
	void run_frame(float dt) {
		system_manager->update_all(dt);
	}
	
protected:

	bool running = false;
	
	EntityManager::Ptr entity_manager;
	EventManager::Ptr event_manager;
	SystemManager::Ptr system_manager;
	ScriptingManager::Ptr scripting_manager;
	
	World::Ptr world;
	
};
