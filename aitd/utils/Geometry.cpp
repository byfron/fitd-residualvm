#include "Geometry.hpp"

namespace Geometry {

bool linePlaneIntersection(Vec3f ray, Vec3f rayOrigin, Vec3f normal,
							   Vec3f planePoint, Vec3f & contact) {

	ray.normalize();
	float d = normal.dot(planePoint);

	if (normal.dot(ray) == 0) {
		return false; // No intersection, the line is parallel to the plane
	}

	// Compute the X value for the directed line ray intersecting the plane
	float x = (d - normal.dot(rayOrigin)) / normal.dot(ray);

	// output contact point
	contact = rayOrigin + ray*x; //Make sure your ray vector is normalized
	return true;
}

Vec3f compute_nearest(const std::vector<Vec3f>& candidates, const Vec3f& point) {
	assert(candidates.size() > 0);
	Vec3f nearest;
	float min_dist = MAX_FLOAT;
	for (auto c : candidates) {
		float dist = (c - point).norm();
		if (dist < min_dist) {
			nearest = c;
			min_dist = dist;
		}
	}
	return nearest;
}
	
Vec3f computeVectorToCollision(const BBox& b1, const BBox& b2, const Vec3f& ray) {

	/*
	 b1 + mv intersects with b2
	 compute the new mv so that b1 does not intersect. 
	 In principle all bboxes are aligned in XZ
	 
        b1                  
	 +------+         	 +------+         
	 |     \| 	         |      | 
	 |      \       	 |      |       
	 |      |\ (mv)      |      |\ (ret) 
	 +------+ \       	 +------+ \
	           +----               +---
               | b2                | 
	*/

	//get closest corner of b2 to the centroid of b1
	Vec3f centroid = b1.getCentroid();
	std::vector<Vec3f> corners = { Vec3f(b2.p_min(0), 0.0, b2.p_min(2)),
								   Vec3f(b2.p_min(0), 0.0, b2.p_max(2)),
								   Vec3f(b2.p_max(0), 0.0, b2.p_min(2)),
								   Vec3f(b2.p_max(0), 0.0, b2.p_max(2)) };
	Vec3f nearest_a_corner = compute_nearest(corners, centroid);

	// Simple First Version: assume displacement in Z plane
	// Run intersection of ray with the 4 edges of the bbox and get the closest hit
	Line line(nearest_a_corner, nearest_a_corner + ray);
		
	Line line_1(Vec3f(b1.p_min(0), 0.0, b1.p_min(2)), Vec3f(b1.p_min(0), 0.0f, b1.p_max(2)));
	Line line_2(Vec3f(b1.p_min(0), 0.0, b1.p_min(2)), Vec3f(b1.p_max(0), 0.0f, b1.p_min(2)));
	Line line_3(Vec3f(b1.p_max(0), 0.0, b1.p_max(2)), Vec3f(b1.p_min(0), 0.0f, b1.p_max(2)));
	Line line_4(Vec3f(b1.p_max(0), 0.0, b1.p_max(2)), Vec3f(b1.p_max(0), 0.0f, b1.p_min(2)));

	std::vector<Vec3f> hitv(4);
	line.intersect(line_1, hitv[0]);
	line.intersect(line_2, hitv[1]);
	line.intersect(line_3, hitv[2]);
	line.intersect(line_4, hitv[3]);

	Vec3f closest_hit = compute_nearest(hitv, nearest_a_corner);

	return closest_hit - nearest_a_corner;
}

Eigen::Matrix3f getXRotMat(float c, float s) {
	Eigen::Matrix3f rot = Eigen::Matrix3f::Identity();
	rot(1,1) = c; // 1  0  0 
	rot(2,2) = c; // 0  c -s 
	rot(2,1) = s; // 0  s  c 
	rot(1,2) = -s;
	return rot;	
}

Eigen::Matrix3f getYRotMat(float c, float s) {
	Eigen::Matrix3f rot = Eigen::Matrix3f::Identity();
	rot(0,0) = c;  // c  0 -s
	rot(2,2) = c;  // 0  1  0
	rot(2,0) = s;  // s  0  c
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
