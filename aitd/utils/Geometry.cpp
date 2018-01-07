#include "Geometry.hpp"

namespace Geometry {

Eigen::Matrix3f getXRotMat(float c, float s) {
	Eigen::Matrix3f rot = Eigen::Matrix3f::Identity();
	rot(1,1) = c; // 1  0  0  // 1  0  0  // 1  0  0  
	rot(2,2) = c; // 0  c -s  // 0  c  s  // 0  s -c
	rot(2,1) = s; // 0  s  c  // 0 -s  c  // 0  c  s
	rot(1,2) = -s;
	return rot;	
}

Eigen::Matrix3f getYRotMat(float c, float s) {
	Eigen::Matrix3f rot = Eigen::Matrix3f::Identity();
	rot(0,0) = c;  // c  0 -s    c  0  s  // s  0  -c
	rot(2,2) = c;  // 0  1  0    0  1  0  // 0  1   0
	rot(2,0) = s;  // s  0  c   -s  0  c  // c  0   s
	rot(0,2) = -s;
	return rot;	
}
	
Eigen::Matrix3f getZRotMat(float c, float s) {
	Eigen::Matrix3f rot = Eigen::Matrix3f::Identity();
	rot(0,0) = c;
	rot(1,1) = c;
	rot(1,0) = s;
	rot(0,1) = -s;
	return rot;	
}

}
