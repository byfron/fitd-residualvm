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

	uint16 num_of_points = READ_LE_UINT16(data);
	data += 2;

	std::vector<Eigen::Vector2i> unsorted_points;
	Vector2i center = Vector2i::Zero();
	for(uint16 point_idx = 0; point_idx < num_of_points; point_idx++) {
		Vector2i point;
		point(0) = (int16)READ_LE_UINT16(data);
		data += 2;
		point(1) = (int16)READ_LE_UINT16(data);
		data += 2;		
		unsorted_points.push_back(point * 10);
		center += point;
	}
	      
	//order points by angle
	center /= num_of_points;
	struct Pair {
		float angle;
		int idx;
		bool operator<(const Pair& a) {
			return angle < a.angle;
		}
	};
	
	std::vector<Pair> angle_vector(num_of_points);	
	for(uint16 point_idx = 0; point_idx < num_of_points; point_idx++) {
		Vector2i v1 = unsorted_points[point_idx] - center;
		float angle = atan2(v1(0), v1(1));
		angle_vector[point_idx] = Pair{angle, point_idx};
	}
	std::sort(angle_vector.begin(), angle_vector.end());
	
	points.clear();
	for (auto p : angle_vector) {
		points.push_back(unsorted_points[p.idx]);
	}
	points.push_back(points[0]);

}

CameraZone::CameraZone(const char* data, const char* base_data) {
	load(data, base_data);
}

void CameraZone::load(const char* data, const char* base_data) {
	dummy1 = READ_LE_UINT16(data + 0x00); //room num. 
	dummy2 = READ_LE_UINT16(data + 0x02);
	dummy3 = READ_LE_UINT16(data + 0x04);
	dummy4 = READ_LE_UINT16(data + 0x06);
	dummy5 = READ_LE_UINT16(data + 0x08);
	dummy6 = READ_LE_UINT16(data + 0x0A);

	const char *p_zone_data = base_data + dummy3;

	uint16 num_zones = READ_LE_UINT16(p_zone_data);
	p_zone_data += 2;
	
	for(int j = 0; j < num_zones; j++) {
		CameraZoneEntry::Ptr czentry =
			CameraZoneEntry::Ptr(new CameraZoneEntry(p_zone_data));
		p_zone_data += 2 + (4 * (czentry->points.size()-1));
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
	int16 y = -READ_LE_UINT16(data + 0x08); // -(world_y - y) * 10
	int16 z = READ_LE_UINT16(data + 0x0A); //(world_z - z) * 10
	
	position = Vec3f(float(x), float(y), float(z))*10;
	
	focal1 = READ_LE_UINT16(data + 0x0C);
	focal2 = READ_LE_UINT16(data + 0x0E);
	focal3 = READ_LE_UINT16(data + 0x10);

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
	float sinx = DataParsing::computeSin(alpha&0x3FF);
	float siny = DataParsing::computeSin(beta&0x3FF);
	float sinz = DataParsing::computeSin(gamma&0x3FF);
	
	transform = Eigen::Matrix4f::Identity();

	Eigen::Matrix3f rotX = Geometry::getXRotMat(sinx, cosx);
	Eigen::Matrix3f rotY = Geometry::getYRotMat(siny, -cosy);
	Eigen::Matrix3f rotZ = Geometry::getZRotMat(sinz, cosz);
		
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
	
	int16 num_camera_zone_def = READ_LE_UINT16(data + 0x12);
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
