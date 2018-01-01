#pragma once

#include <graphics/GraphicsEngine.hpp>
#include <Eigen/Dense>
#include <bgfx/bgfx.h>
#include "Shader.hpp"


#define PRIM_TYPE_LINE 0
#define PRIM_TYPE_POLYGON 1
#define PRIM_TYPE_SPHERE 2

using namespace Eigen;

namespace Geometry {

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
