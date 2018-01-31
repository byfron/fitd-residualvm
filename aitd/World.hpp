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

	//Maybe this should go to a factory class
	Entity createCameraEntity(Floor::Ptr floor_data, int room_index, int camera_index);

	void switchToCamera(Entity::Id camera_id);
	
	Entity::Id getCurrentCameraId() const { return current_camera_id; }
	
protected:

	std::map<int, Floor::Ptr> floor_map;
	EntityManager::Ptr entity_manager;
	uint32_t current_floor_id;
	uint32_t current_room_id;


	Entity::Id current_camera_id;

};

