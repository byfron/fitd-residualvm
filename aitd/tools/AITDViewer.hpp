#pragma once
#include <ActorLoader.hpp>
#include <entities/Entity.hpp>

class AITDViewer {
public:

	~AITDViewer();
	void init();
	void run_frame(float dt);	
	
protected:
	
	EntityManager::Ptr entity_manager;
};
