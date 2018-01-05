#pragma once

#include "Floor.hpp"
#include <entities/Entity.hpp>
#include <map>

class World {
public:
	typedef std::shared_ptr<World> Ptr;
	World(EntityManager::Ptr em) : entity_manager(em) {}	   
	void load();
	void render(float dt);
	
protected:

	std::map<int, Floor::Ptr> floor_map;
	EntityManager::Ptr entity_manager;
	Entity::Id current_camera_id;
	Entity::Id current_room_id;
};
