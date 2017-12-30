#pragma once

#include "Room.hpp"
#include "RoomCamera.hpp"

class Floor {

public:

	void load(int floor_idx);

protected:

	std::vector<Room::Ptr> room_vector;
	std::vector<RoomCamera::Ptr> camera_vector;
};

