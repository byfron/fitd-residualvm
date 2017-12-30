#pragma once

#include <memory>
#include <Eigen/Dense>
#include <common/scummsys.h>
#include <common/endian.h>

typedef Eigen::Matrix<int,3,1> Vector3i;

class BBox {
public:
	typedef std::shared_ptr<BBox> Ptr;
	BBox(const char*);
	void load(const char*);	

protected:
	Vector3i p1;
	Vector3i p2;
	int16 parameter;
	int16 type;
};

class Room {
public:

	typedef std::shared_ptr<Room> Ptr;

	void load(const char *data);

protected:

	std::vector<BBox::Ptr> colision_vector;
	std::vector<BBox::Ptr> sce_zone_vector;
	std::vector<int> camera_indices;
	Vector3i world_pos;
	
   
};
