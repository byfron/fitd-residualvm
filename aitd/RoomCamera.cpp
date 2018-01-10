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

 	uint16 alpha = READ_LE_UINT16(data + 0x00);
	uint16 beta  = READ_LE_UINT16(data + 0x02);
	uint16 gamma = READ_LE_UINT16(data + 0x04);
	
	int16 x = READ_LE_UINT16(data + 0x06); // (x - world_x) * 10
	int16 y = READ_LE_UINT16(data + 0x08); // (world_y - y) * 10
	int16 z = -READ_LE_UINT16(data + 0x0A); // (world_z - z) * 10
	
	position = Vec3f(float(x), float(y), float(z))*10;
	
	focal1 = READ_LE_UINT16(data + 0x0C);
	focal2 = READ_LE_UINT16(data + 0x0E);
	focal3 = READ_LE_UINT16(data + 0x10);

	std::cout << alpha << "," << beta << "," << gamma << std::endl;
	std::cout << x << "," << y << "," << z << std::endl;
	std::cout << focal1 << "," << focal2 << "," << focal3 << std::endl;

	
	// Build projection matrix!
	float fx = focal2;
	float fy = focal3;
	float fz = focal1; // no idea where this param comes from or what is for (*)
	float s = 0;
	float zmin = 0;
	float zmax = 1000000;	
	float W = GraphicsEngine::WIDTH;
	float H = GraphicsEngine::HEIGHT;
	float cx = GraphicsEngine::WIDTH/2;
	float cy = GraphicsEngine::HEIGHT/2;
	
	projection = Eigen::Matrix4f::Zero();
	projection(0,0) = 2*fx/W;
	projection(0,1) = 2*s/W;
	projection(1,1) = 2*fy/H;
	projection(0,2) = 2*(cx/W)-1;
	projection(1,2) = 2*(cy/H)-1;
	projection(2,2) = (zmax+zmin)/(zmax-zmin);
	projection(3,2) = 1;
	projection(2,3) = 2*zmax*zmin/(zmin-zmax);
	projection(3,3) = fz; // (*) but has to go here

	// Build view matrix
	float cosx = DataParsing::computeCos(alpha&0x3FF);
	float cosy = DataParsing::computeCos(beta&0x3FF);
	float cosz = DataParsing::computeCos(gamma&0x3FF);
	
	transform = Eigen::Matrix4f::Identity();
	
	// The angles are shifted!! Use the arcsin instead.
	float angle_x = asinf(cosx);
	float angle_y = asinf(cosy);
	float angle_z = asinf(cosz);
	if (alpha >= 0x3FF) angle_x = 0;
	if (beta >= 0x3FF) angle_y = 0;
	if (gamma >= 0x3FF) angle_z = 0;

	Eigen::Matrix3f rotX = Eigen::Matrix3f(AngleAxisf(angle_x, Vector3f::UnitX()));
	Eigen::Matrix3f rotY = Eigen::Matrix3f(AngleAxisf(angle_y, Vector3f::UnitY()));
	Eigen::Matrix3f rotZ = Eigen::Matrix3f(AngleAxisf(angle_z, Vector3f::UnitZ()));
	
	if (!beta) {
		rotY.col(0) = Vector3f::UnitX();
		rotY.col(2) = Vector3f::UnitZ();
		rotY = Eigen::Matrix3f::Identity();
	}
	if (!alpha) {
		rotX.col(1) = Vector3f::UnitY();
		rotX.col(2) = Vector3f::UnitZ();
		rotX = Eigen::Matrix3f::Identity();
	}
	if (!gamma) {
		rotZ.col(0) = Vector3f::UnitX();
		rotZ.col(1) = Vector3f::UnitY();
		rotZ = Eigen::Matrix3f::Identity();
	}

	Eigen::Matrix3f t = Eigen::Matrix3f(rotZ * rotY * rotX);	
	transform.topLeftCorner(3,3) = t;
	transform.col(3).head(3) = position;
	
	int num_camera_zone_def = READ_LE_UINT16(data + 0x12);
	const char* base_data = data;
	data += 0x14;
	
	for(int k = 0; k < num_camera_zone_def; k++) {
		CameraZone::Ptr zone = CameraZone::Ptr(new CameraZone(data, base_data));
		zone_vector.push_back(zone);
		data += 0x0C;
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
