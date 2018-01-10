#pragma once

#include "Floor.hpp"
#include "ObjectManager.hpp"
#include <entities/Entity.hpp>
#include <map>

class World {
public:
	typedef std::shared_ptr<World> Ptr;
	World(EntityManager::Ptr em) : entity_manager(em) {}	   
	void load();
	void render(float dt);

	void loadFloor(int);

	void createObjectEntities(const ObjectData& object);
	
protected:

	std::map<int, Floor::Ptr> floor_map;
	EntityManager::Ptr entity_manager;
	uint32_t current_floor_id;
	uint32_t current_camera_id;
	uint32_t current_room_id;
};
