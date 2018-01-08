#pragma once
#include <ActorLoader.hpp>
#include <entities/Entity.hpp>

class AITDViewer {
public:

	~AITDViewer();
	void init();
	void run_frame(float dt);	
	
protected:
	
	Actor::Ptr actor_data;
	EntityManager::Ptr entity_manager;
};
