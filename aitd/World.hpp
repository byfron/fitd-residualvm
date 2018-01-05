#pragma once

#include "Floor.hpp"
#include <map>

class World {
public:
	typedef std::shared_ptr<World> Ptr;
	void load();
	void render(float dt);
	
protected:

	std::map<int, Floor::Ptr> floor_map;

};
