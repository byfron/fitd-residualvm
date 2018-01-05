#include "World.hpp"

void World::load() {
	//TODO  Floor:Ptr floor_data = FloorFactory::create(0);
	Floor::Ptr floor_data = Floor::Ptr(new Floor());
	floor_data->load(0);
	
}

void World::render(float dt) {
//	floor_data->getCamera(0);
}
