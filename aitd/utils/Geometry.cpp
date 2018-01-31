#include "Geometry.hpp"

namespace Geometry {

#define INF 100000000	
	
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

bool onSegment(const Eigen::Vector2i& p, const Eigen::Vector2i& q, const Eigen::Vector2i& r) {

	if (q(0) <= std::max(p(0), r(0)) &&
		q(0) >= std::min(p(0), r(0)) &&
		q(1) <= std::max(p(1), r(1)) &&
		q(1) >= std::min(p(1), r(1))) {
		return true;
	}
	return false;
}

int orientation(const Eigen::Vector2i& p, const Eigen::Vector2i& q, const Eigen::Vector2i& r) {
	int val = (q(1) - p(1)) * (r(0) - q(0)) -
		(q(0) - p(0)) * (r(1) - q(1));

	if (val == 0) return 0; //colinear
	return (val > 0)? 1: 2; //clock / counter-wise clock
}

bool doIntersect(const Eigen::Vector2i& p1, const Eigen::Vector2i& q1,
				 const Eigen::Vector2i& p2, const Eigen::Vector2i& q2) {

	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	if (o1 != o2 && o3 != o4) {
		return true;
	}

	if (o1 == 0 && onSegment(p1, p2, q1)) return true;
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false;
}
bool Polygon::isWithin(const Eigen::Vector2i& point) {

	size_t n = points.size();
	bool result = false;
	for (size_t i = 0; i < n; ++i) {
		size_t j = (i + 1) % n;

		if (
			// Does p0(1) lies in half open y range of edge.
			// N.B., horizontal edges never contribute
			( (points[j](2) <= point(1) && point(1) < points[i](2)) || 
			  (points[i](2) <= point(1) && point(1) < points[j](2)) ) &&
			// is p to the left of edge?
			( point(0) < points[j](0) + (points[i](0) - points[j](0)) *
			  float(point(1) - points[j](2)) / (points[i](2) - points[j](2)) )
			)
			result = !result;
	}
	return result;
}
	
}
