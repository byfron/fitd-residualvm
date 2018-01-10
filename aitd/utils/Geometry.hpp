#pragma once

#include <graphics/GraphicsEngine.hpp>
#include <Eigen/Dense>
#include <bgfx/bgfx.h>
#include "Shader.hpp"
#include <DebugManager.hpp>
#include <vector>


#define PRIM_TYPE_LINE 0
#define PRIM_TYPE_POLYGON 1
#define PRIM_TYPE_SPHERE 2

using namespace Eigen;

namespace Geometry {

Eigen::Matrix3f getXRotMat(float, float);
Eigen::Matrix3f getYRotMat(float, float);
Eigen::Matrix3f getZRotMat(float, float);
	
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

	
class Primitive {
public:
	virtual ~Primitive() = default;
	
	typedef std::shared_ptr<Primitive> Ptr;
	Primitive(int c, int t) : color(c), type(t) {}
	int type = 0;
	int color = 0;
};


// The primivitves store pointers to the pool of vertices
// so that it's easier to deal with animations
class Line : public Primitive {
public:
	Line(const Vector3f* a, const Vector3f* b, int c) :
		Primitive(c, PRIM_TYPE_LINE),
		point_a(a),
		point_b(b) {}
	const Vector3f* point_a;
	const Vector3f* point_b;
};

class Polygon : public Primitive {
public:
	Polygon(const std::vector<const Vector3f*>& p, int t, int c) :
		Primitive(c, PRIM_TYPE_POLYGON),
		poly_type(t),
		points(p) {}
	std::vector<const Vector3f*> points;
	int poly_type = 0;
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
