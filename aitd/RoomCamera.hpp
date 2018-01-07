#pragma once

#include "math.hpp"
#include <memory>
#include <Eigen/Dense>
#include <common/scummsys.h>
#include <vector>

class BgImage {

	
	unsigned char* background_image;
};

class CameraZoneEntry {
public:
	typedef std::shared_ptr<CameraZoneEntry> Ptr;
	CameraZoneEntry(const char*);
	void load(const char*);
	std::vector<Eigen::Vector2i> points;
};

class CameraZone {
public:
	typedef std::shared_ptr<CameraZone> Ptr;
	CameraZone(const char*, const char*);
	void load(const char*, const char*);
protected:
	int16 dummy1;
	int16 dummy2;
	int16 dummy3;
	int16 dummy4;
	int16 dummy5;
	int16 dummy6;
	std::vector<CameraZoneEntry::Ptr> entry_vector;
};
	
class RoomCamera {
public:
	~RoomCamera();
	typedef std::shared_ptr<RoomCamera> Ptr;
	RoomCamera(const char*);
	void load(const char*);
	void loadBackgroundImage(const char*);

	unsigned char* getBackgroundImagePtr() {
		return background_image;
	}
	
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
	std::vector<CameraZone::Ptr> zone_vector;

	int16 alpha;
	int16 beta;
 	int16 gamma;

	int16 focal1;
	int16 focal2;
	int16 focal3;

	Vec3f position;
	Vec3f look_at;
	Eigen::Matrix4f transform;
	Eigen::Matrix4f projection;
	
	unsigned char * background_image;
};
