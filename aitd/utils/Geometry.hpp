#pragma once

#include <graphics/GraphicsEngine.hpp>
#include <Eigen/Dense>
#include <bgfx/bgfx.h>
#include "Shader.hpp"
#include <DebugManager.hpp>
#include <vector>
#include <assert.h>

#define PRIM_TYPE_LINE 0
#define PRIM_TYPE_POLYGON 1
#define PRIM_TYPE_SPHERE 2

using namespace Eigen;

namespace Geometry {

#define MAX_FLOAT 0xFFFFFFFF

struct Point {
	Point(Eigen::Vector2i& p) : x(p(0)), y(p(1)) {}
	float x;
	float y;
};
	
class Line {
public:
	Line(const Vec3f& _p1, const Vec3f& _p2) : p1(_p1), p2(_p2) {

	}

	bool intersect(const Line& line, Vec3f& hit) const {
		
	}
	
	Vec3f p1;
	Vec3f p2;
};
	
class Plane {
public:
	Plane(const Vec3f& p, const Vec3f& n) : point(p), normal(n) {}
	Vec3f point;
	Vec3f normal;
};


template <class T>	
class Polygon {
public:
	bool isWithin(const T& point) {		
		size_t n = points.size();
		bool result = false;
		for (size_t i = 0; i < n; ++i) {
			size_t j = (i + 1) % n;

			if (
				// Does p0(1) lies in half open y range of edge.
				// N.B., horizontal edges never contribute
				( (points[j](1) <= point(1) && point(1) < points[i](1)) || 
				  (points[i](1) <= point(1) && point(1) < points[j](1)) ) &&
				// is p to the left of edge?
				( point(0) < points[j](0) + (points[i](0) - points[j](0)) *
				  float(point(1) - points[j](1)) / (points[i](1) - points[j](1)) )
				)
				result = !result;
		}
		return result;
	}

	Polygon(std::vector<Eigen::Vector2i>& ipoints) {
		for (auto ip : ipoints) {
			points.push_back(T(ip(0), ip(1)));
		}
	}
	Polygon(const std::vector<T>& p) : points(p) {}
	std::vector<T> points;
};

class BBox {
public:
	BBox() {}
	BBox(const Vec3f& a, const Vec3f& b) : p_min(a), p_max(b) {
		assert(a(0) < b(0) && a(1) < b(1) && a(2) < b(2));
	}

	BBox getTransformedBox() const {

		return BBox(p_min + translation,
					p_max + translation);
	}
	
	Vec3f getCentroid() const {
		return (p_max + p_min)/2.0f;
	}

	bool intersect(const Vec3f& point) const {
		
		if (point(0) > p_min(0) && point(1) > p_min(1) && point(2) > p_min(2) &&
			point(0) < p_max(0) && point(1) < p_max(1) && point(2) < p_max(2)) {
			return true;
		}
		return false;
	}
	
	uint32_t intersect(const BBox& box) const {

		const uint32_t ltMinX = p_max(0) <  box.p_min(0);
		const uint32_t gtMaxX = p_min(0) >  box.p_max(0);
		const uint32_t ltMinY = p_max(1) <  box.p_min(1);
		const uint32_t gtMaxY = p_min(1) >  box.p_max(1);
		const uint32_t ltMinZ = p_max(2) <  box.p_min(2);
		const uint32_t gtMaxZ = p_min(2) >  box.p_max(2);
		return 0
			| (ltMinX <<0)
			| (gtMaxX <<1)
			| (ltMinY <<2)
			| (gtMaxY <<3)
			| (ltMinZ <<4)
			| (gtMaxZ <<5);		
	}

	Eigen::Vector3f translation = Eigen::Vector3f::Zero();
	Vec3f p_min;
	Vec3f p_max;
};
	
Eigen::Matrix3f getXRotMat(float, float);
Eigen::Matrix3f getYRotMat(float, float);
Eigen::Matrix3f getZRotMat(float, float);

Vec3f computeVectorToCollision(const BBox&, const BBox&, const Vec3f&);	
bool linePlaneIntersection(Vec3f ray, Vec3f rayOrigin, Vec3f normal,
						   Vec3f planePoint, Vec3f & contact);

class DebugMesh {
public:
	typedef std::shared_ptr<DebugMesh> Ptr;
	virtual ~DebugMesh() {}
	virtual void render(float dt) = 0;			
};

class DebugBox : public DebugMesh {
public:
	DebugBox(const Vec3f& pa, const Vec3f& pb ) :
		point_a(pa),
		point_b(pb) {}

	void render(float dt) override {
		DebugManager::push_aabb(point_a, point_b);
	}
protected:

	Vec3f point_a;
	Vec3f point_b;	
};

class DebugLine : public DebugMesh {
public:
	DebugLine(const Vec3f& pa, const Vec3f& pb ) :
		point_a(pa),
		point_b(pb) {}

	void render(float dt) override {
		DebugManager::push_line(point_a, point_b);
	}
protected:

	Vec3f point_a;
	Vec3f point_b;	
};

class DebugAxis : public DebugMesh {
public:
	DebugAxis(const Eigen::Matrix4f& transform) {
		center = transform.col(3).head(3);
		axis_x = transform.topLeftCorner(3,3) * Vec3f::UnitX() * 1000;
		axis_y = transform.topLeftCorner(3,3) * Vec3f::UnitY() * 1000;
		axis_z = transform.topLeftCorner(3,3) * Vec3f::UnitZ() * 1000;
	}

	void render(float dt) override {
		DebugManager::push_line(center, center + axis_x, 0xff0000ff); //ABGR
		DebugManager::push_line(center, center + axis_y, 0xff00ff00);
		DebugManager::push_line(center, center + axis_z, 0xffff0000);
	}

protected:
	Vec3f center;
	Vec3f axis_x;
	Vec3f axis_y;
	Vec3f axis_z;
};
	
class DebugSphere : public DebugMesh {
public:
	DebugSphere(const Vec3f& c, float r) {

		sphere.m_center[0] = c(0);
		sphere.m_center[1] = c(1);
		sphere.m_center[2] = c(2);
		sphere.m_radius = r;
	}
	
	void render(float dt) override {
		DebugManager::push_sphere(sphere);
	}
protected:
	
	Sphere sphere;
};

class Mesh {
public:

	typedef std::shared_ptr<Mesh> Ptr;
	
	Mesh() {

	}

	void submit() {
		bgfx::setIndexBuffer(m_dibh);
		bgfx::setVertexBuffer(0, m_dvbh);

		// Set render states.
		bgfx::setState(0
					   | BGFX_STATE_RGB_WRITE
					   | BGFX_STATE_ALPHA_WRITE
					   | BGFX_STATE_DEPTH_WRITE
					   | BGFX_STATE_DEPTH_TEST_LESS
					   | BGFX_STATE_MSAA);
		
		bgfx::submit(RENDER_PASS_GEOMETRY,
					 m_shader->getHandle());
	}
	
	bgfx::DynamicVertexBufferHandle m_dvbh;
	bgfx::DynamicIndexBufferHandle m_dibh;
	Shader::Ptr         m_shader;

};

}
