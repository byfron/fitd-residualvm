#include "Floor.hpp"
#include "Room.hpp"
#include <file_access.h>
#include <fitd.h>
#include <resource.h>
#include <pak.h>
#include <iostream>

int getNumCameras(char* data) {
	return ((READ_LE_UINT32(data)) / 4);
}

int getNumRooms(char* data, int data_size) {
	int j = 0;
	int num_max = (((READ_LE_UINT32(data)) / 4));
	for(int i = 0; i < num_max; i++) {
		if(data_size >= READ_LE_UINT32(data + i * 4)) {
			j++;
		} else {
			return j;
		}
	}
	return j;
}

using namespace Fitd;
void Floor::load(int floor_idx) {

	// TODO: Refactor loading code into Factories!!

	char floor_file[256];
	sprintf(floor_file, "ETAGE%02d", floor_idx);

	int floor_data_size = getPakSize(floor_file, 0);
	int camera_data_size = getPakSize(floor_file, 1);
	
	char *floor_data = g_resourceLoader->loadPakSafe(floor_file, 0);
	char *camera_data = g_resourceLoader->loadPakSafe(floor_file, 1);

	int num_rooms = getNumRooms(floor_data, floor_data_size);

	//TODO: add spdlog
	std::cout << "num rooms:" << num_rooms << std::endl;

	// Load rooms
	for (int i = 0; i < num_rooms; i++) {
		char* room_data = (floor_data + READ_LE_UINT32(floor_data + i * 4));
		Room::Ptr room = Room::Ptr(new Room());

		std::cout << "Loading room " << i << std::endl;
		
		room->load(room_data);
		room_vector.push_back(room);
	}

	char camera_file[256];
	sprintf(camera_file, "CAMERA%02d", floor_idx);

	// Load cameras
	int num_cameras = getNumCameras(camera_data);
	for (int i = 0; i < num_cameras; i++) {
		uint32 offset = READ_LE_UINT32(camera_data + i * 4);

		std::cout << "camera:" << i << std::endl;
		std::cout << "offset:" << offset << std::endl;
		if (offset < camera_data_size) {
			char *current_camera_data = camera_data + offset;
			RoomCamera::Ptr camera = RoomCamera::Ptr(new RoomCamera(current_camera_data));
			
			char camera_bg_data[65068];
			if(!loadPakToPtr(camera_file, i, camera_bg_data)) {
				theEnd(0, camera_file);
			}
			else {
				camera->loadBackgroundImage(camera_bg_data);
			}
			
			camera_map[i] = camera;
		}
		else {
			// warning?
		}
	}
	
}
