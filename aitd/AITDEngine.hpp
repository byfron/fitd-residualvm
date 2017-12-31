#pragma once

#include "Floor.hpp"
#include "Actor.hpp"
#include "entities/Entity.hpp"
#include "entities/System.hpp"
#include <map>

class AITDEngine {
public:
	~AITDEngine();
	void init();
	void createSubsystems();
	void loadGameData();
	void requestQuit();
	
	template <typename S>
	void add(std::shared_ptr<S> system) {
		systemManager->add<S>(system);
	}
	
	void run_frame(float dt) {
//		systemManager->update_all(dt);

		for (auto id_actor : actor_map) {
			id_actor.second->draw(dt);
		}
	}
	
protected:

	bool running = false;
	
	EntityManager::Ptr entityManager;
	EventManager::Ptr eventManager;
	SystemManager::Ptr systemManager;
	
	std::map<int, Floor::Ptr> floor_map;
	std::map<int, Actor::Ptr> actor_map;
	
};
