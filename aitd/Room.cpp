#include "Room.hpp"
#include <iostream>

BBox::BBox(const char* data) {
	load(data);
}

void BBox::load(const char* data) {
	p1(0) = (int16)READ_LE_UINT16(data + 0x00);
	p2(0) = (int16)READ_LE_UINT16(data + 0x02);
	p1(1) = (int16)READ_LE_UINT16(data + 0x04);
	p2(1) = (int16)READ_LE_UINT16(data + 0x06);
	p1(2) = (int16)READ_LE_UINT16(data + 0x08);
	p2(2) = (int16)READ_LE_UINT16(data + 0x0A);

//	p1 /= 1000.0f;
//	p2 /= 1000.0f;
	
	parameter = READ_LE_UINT16(data + 0x0C);
	type = READ_LE_UINT16(data + 0x0E);
}

// TODO: All these eventually become Loaders and all the logic stays into components
void Room::load(const char *data) {

	int16 wx = (int16)READ_LE_UINT16(data + 4);
	int16 wy = (int16)READ_LE_UINT16(data + 6);
	int16 wz = (int16)READ_LE_UINT16(data + 8);
	
	world_pos(0) = static_cast<float>((int16)READ_LE_UINT16(data + 4));
	world_pos(1) = static_cast<float>((int16)READ_LE_UINT16(data + 6));
	world_pos(2) = static_cast<float>((int16)READ_LE_UINT16(data + 8));

//	std::cout << "pos:" << world_pos.transpose() << std::endl;
	
	int num_cameras = READ_LE_UINT16(data + 0xA);

	for(uint32 j = 0; j < num_cameras; j++) {
		int16 camera_idx = READ_LE_UINT16(data + 0xC + 2 * j);
		camera_indices.push_back(camera_idx);

		std::cout << "camera idx:" << camera_idx << std::endl;
	}

	//room structure data
	const char *hard_col_data = data + READ_LE_UINT16(data);
	int num_hard_cols = READ_LE_UINT16(hard_col_data);
	hard_col_data += 2;

	if(num_hard_cols) {	
	
		// Box transform expressed wrt parent
		for(uint32 j = 0; j < num_hard_cols; j++) {
			BBox::Ptr bbox = BBox::Ptr(new BBox(hard_col_data));

			// See note in World.cpp about the scene tree
			Vector3i p1 = bbox->p1;
			Vector3i p2 = bbox->p2;

			p1(0) = p1(0) + wx;
			p1(1) = -(p1(1) + wy);
			p1(2) = p1(2) + wz;
			p2(0) = p2(0) + wx;
			p2(1) = -(p2(1) + wy);
			p2(2) = p2(2) + wz;

			
			bbox->p1 = p1;
			bbox->p2 = p2;
			
			colision_vector.push_back(bbox);
			hard_col_data += 0x10; //16
		}

		// Triggers
		const char *sce_zone_data = data + READ_LE_UINT16(data + 2);
		int num_sce_zones = READ_LE_UINT16(sce_zone_data);
		sce_zone_data += 2;
	
		for(uint32 j = 0; j < num_sce_zones; j++) {
			BBox::Ptr bbox = BBox::Ptr(new BBox(hard_col_data));
			sce_zone_vector.push_back(bbox);
			sce_zone_data += 0x10; //16

		}
	}	
}
