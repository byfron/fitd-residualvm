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

Eigen::Matrix3d getXRotMat(float, float);
Eigen::Matrix3d getYRotMat(float, float);
Eigen::Matrix3d getZRotMat(float, float);

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
	
	
class Primitive {
public:
	virtual ~Primitive() = default;
	
	typedef std::shared_ptr<Primitive> Ptr;
	Primitive(int c, int t) : color(c), type(t) {}
	int type = 0;
	int color = 0;
};

class Line : public Primitive {
public:
	Line(const Vector3f& a, const Vector3f& b, int c) :
		Primitive(c, PRIM_TYPE_LINE),
		point_a(a),
		point_b(b) {}
	Vector3f point_a;
	Vector3f point_b;
};

class Polygon : public Primitive {
public:
	Polygon(const std::vector<Vector3f>& p, int t, int c) :
		Primitive(c, PRIM_TYPE_POLYGON),
		poly_type(t),
		points(p) {}
	std::vector<Vector3f> points;
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
