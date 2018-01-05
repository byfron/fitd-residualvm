#include "Geometry.hpp"

namespace Geometry {

Eigen::Matrix3d getXRotMat(float c, float s) {
	Eigen::Matrix3d rot = Eigen::Matrix3d::Identity();
	rot(1,1) = c;
	rot(2,2) = c;
	rot(2,1) = s;
	rot(1,2) = -s;
	return rot;	
}

Eigen::Matrix3d getYRotMat(float c, float s) {
	Eigen::Matrix3d rot = Eigen::Matrix3d::Identity();
	rot(0,0) = c;
	rot(2,2) = c;
	rot(2,0) = -s;
	rot(0,2) = s;
	return rot;	
}
	
Eigen::Matrix3d getZRotMat(float c, float s) {
	Eigen::Matrix3d rot = Eigen::Matrix3d::Identity();
	rot(0,0) = c;
	rot(1,1) = c;
	rot(1,0) = s;
	rot(0,1) = -s;
	return rot;	
}

}
