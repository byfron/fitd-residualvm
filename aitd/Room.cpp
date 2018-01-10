#include "Room.hpp"
#include <iostream>

BBox::BBox(const char* data) {
	load(data);
}

void BBox::load(const char* data) {
	p1(0) = (int16)READ_LE_UINT16(data + 0x00);
	p2(0) = (int16)READ_LE_UINT16(data + 0x02);
	p1(1) = -(int16)READ_LE_UINT16(data + 0x04);
	p2(1) = -(int16)READ_LE_UINT16(data + 0x06);
	p1(2) = (int16)READ_LE_UINT16(data + 0x08);
	p2(2) = (int16)READ_LE_UINT16(data + 0x0A);

//	p1 /= 1000.0f;
//	p2 /= 1000.0f;
	
	parameter = READ_LE_UINT16(data + 0x0C);
	type = READ_LE_UINT16(data + 0x0E);
}

// TODO: All these eventually become Loaders and all the logic stays into components
void Room::load(const char *data) {

	world_pos(0) = static_cast<float>((int16)READ_LE_UINT16(data + 4));
	world_pos(1) = static_cast<float>((int16)READ_LE_UINT16(data + 6));
	world_pos(2) = static_cast<float>(-(int16)READ_LE_UINT16(data + 8));

	int num_cameras = READ_LE_UINT16(data + 0xA);

	for(uint32 j = 0; j < num_cameras; j++) {
		int16 camera_idx = READ_LE_UINT16(data + 0xC + 2 * j);
		camera_indices.push_back(camera_idx);
	}

	//room structure data
	const char *hard_col_data = data + READ_LE_UINT16(data);
	int num_hard_cols = READ_LE_UINT16(hard_col_data);
	hard_col_data += 2;

	// Box transform expressed wrt parent
	for(uint32 j = 0; j < num_hard_cols; j++) {
		BBox::Ptr bbox = BBox::Ptr(new BBox(hard_col_data));
	    colision_vector.push_back(bbox);
		hard_col_data += 16;
	}

	// Triggers
	const char *sce_zone_data = data + READ_LE_UINT16(data + 2);
	int num_sce_zones = READ_LE_UINT16(sce_zone_data);
	sce_zone_data += 2;
	
	for(uint32 j = 0; j < num_sce_zones; j++) {
		BBox::Ptr bbox = BBox::Ptr(new BBox(hard_col_data));
		sce_zone_vector.push_back(bbox);
		sce_zone_data += 16;

	}

	
}
