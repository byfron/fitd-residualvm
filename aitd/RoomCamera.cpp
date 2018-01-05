#include "RoomCamera.hpp"
#include <common/endian.h>
#include <utils/Color.hpp>
#include <utils/DataParsing.hpp>
#include <utils/Geometry.hpp>

typedef Eigen::Matrix<int,2,1> Vector2i;

CameraZoneEntry::CameraZoneEntry(const char* data) {
	load(data);
}

void CameraZoneEntry::load(const char* data) {
	int num_of_points = READ_LE_UINT16(data);
	data += 2;
	for(int32 point_idx = 0; point_idx < num_of_points; point_idx++) {
		Vector2i point;
		point(0) = READ_LE_UINT16(data);
		data += 2;
		point(1) = READ_LE_UINT16(data);
		data += 2;
		points.push_back(point);
	}
	
	points.push_back(points[0]);
}

CameraZone::CameraZone(const char* data, const char* base_data) {
	load(data, base_data);
}

void CameraZone::load(const char* data, const char* base_data) {
	dummy1 = READ_LE_UINT16(data + 0x00);
	dummy2 = READ_LE_UINT16(data + 0x02);
	dummy3 = READ_LE_UINT16(data + 0x04);
	dummy4 = READ_LE_UINT16(data + 0x06);
	dummy5 = READ_LE_UINT16(data + 0x08);
	dummy6 = READ_LE_UINT16(data + 0x0A);

	const char *p_zone_data = base_data + dummy3;

	int num_zones = READ_LE_UINT16(p_zone_data);
	p_zone_data += 2;

	for(int j = 0; j < num_zones; j++) {
		CameraZoneEntry::Ptr czentry =
			CameraZoneEntry::Ptr(new CameraZoneEntry(p_zone_data));
		p_zone_data += 2 + (4 * czentry->points.size());
		entry_vector.push_back(czentry);
	}
}

RoomCamera::RoomCamera(const char* data) {
	load(data);
}

RoomCamera::~RoomCamera() {
	delete [] background_image;
}

void RoomCamera::load(const char *data) {

	alpha = READ_LE_UINT16(data + 0x00);
	beta  = READ_LE_UINT16(data + 0x02);
	gamma = READ_LE_UINT16(data + 0x04);
	
	int16 x = READ_LE_UINT16(data + 0x06); // (x - world_x) * 10
	int16 y = READ_LE_UINT16(data + 0x08); // (world_y - y) * 10
	int16 z = -READ_LE_UINT16(data + 0x0A); // (world_z - z) * 10

	position = Vec3f(float(x), float(y), float(z))/100.0f; //(1/1000 * 10)
	
	focal1 = READ_LE_UINT16(data + 0x0C);
	focal2 = READ_LE_UINT16(data + 0x0E);
	focal3 = READ_LE_UINT16(data + 0x10);

	float cosx = DataParsing::computeCos(alpha);
	float sinx = DataParsing::computeSin(alpha);
	float cosy = DataParsing::computeCos(beta);
	float siny = DataParsing::computeSin(beta);
	float cosz = DataParsing::computeCos(gamma);
	float sinz = DataParsing::computeSin(gamma);

	transform = Eigen::Matrix4f::Identity();
	// transform.topLeftCorner(3,3) =
	// 	Geometry::getZRotMat(cosx, sinx) *
	// 	Geometry::getYRotMat(cosy, siny) *
	// 	Geometry::getXRotMat(cosz, sinz);

	Eigen::Matrix3f t = Eigen::Matrix3f(AngleAxisf(acosf(cosx), Vector3f::UnitZ())
										* AngleAxisf(acosf(cosy), Vector3f::UnitY())
										* AngleAxisf(acosf(cosz), Vector3f::UnitX()));

	transform.topLeftCorner(3,3) = t;
	transform.col(3).head(3) = position;
		
	//look_at = t * Vec3f(0.0, 1.0, 0.0);
	look_at = transform.topLeftCorner(3,3).cast<float>() * Vec3f(0.0, 0.0, 1.0);
	
	int num_camera_zone_def = READ_LE_UINT16(data + 0x12);


	// std::cout << "Room Camera loaded: " << std::endl;
	// std::cout << "x: " << position(0x << ", y: " << y << ", z: " << z << std::endl;
	// std::cout << "f1: " << focal1 << ", f2: " << focal2 << ", f3: " << focal3 << std::endl;
	// std::cout << "rot: " << transform << std::endl;
	
	const char* base_data = data;
	data += 0x14;
	
	for(int k = 0; k < num_camera_zone_def; k++) {
		CameraZone::Ptr zone = CameraZone::Ptr(new CameraZone(data, base_data));
		zone_vector.push_back(zone);	
	}
}

void RoomCamera::loadBackgroundImage(const char* data) {
	ColorPalette::Ptr color_palette =
		ResourceManager::getResource<ColorPalette>(OBJECT_PALETTE_ID);
	background_image = new unsigned char[320*200*3];

	for (int i = 0; i < 320; i++) {
		for (int j = 0; j < 200; j++) {

			const unsigned char color_index = data[i*200 + j];
			Color c = color_palette->getColor(color_index);

			background_image[(i*200 + j)*3 + 0] = c.r;
			background_image[(i*200 + j)*3 + 1] = c.g;
			background_image[(i*200 + j)*3 + 2] = c.b;
		}
	}
}
