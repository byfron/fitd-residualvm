#pragma once

#include "Room.hpp"
#include "RoomCamera.hpp"

class Floor {

public:

	typedef std::shared_ptr<Floor> Ptr;
	
	void load(int floor_idx);

	RoomCamera::Ptr getCamera(int cam_idx) {
		assert(cam_idx < camera_vector.size());
		return camera_vector[cam_idx];
	}

	Room::Ptr getRoom(int room_idx) {
		assert(room_idx < room_vector.size());
		return room_vector[room_idx];
	}
	
protected:

	std::vector<Room::Ptr> room_vector;
	std::vector<RoomCamera::Ptr> camera_vector;
};

