#pragma once

#include "GraphicsEngine.hpp"
#include "Shader.hpp"
#include <bgfx/bgfx.h>

#include <vector>
#include <map>
#include <Eigen/Dense>
#include <common/endian.h>

#define PRIM_TYPE_LINE 0
#define PRIM_TYPE_POLYGON 1
#define PRIM_TYPE_SPHERE 2

using namespace Eigen;

struct PosColorVertex
{
	float m_x;
	float m_y;
	float m_z;
	uint32_t m_abgr;

	static void init()
	{
		ms_decl
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
			.end();
	};

	static bgfx::VertexDecl ms_decl;
};


class Bone {
public:

	Bone* parent;
	Eigen::Quaternionf local_rot;
	Eigen::Vector3f local_pos;
	
};

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
	Polygon(const std::vector<Vector3f>& p, int c) :
		Primitive(c, PRIM_TYPE_POLYGON),
		points(p) {}
	std::vector<Vector3f> points;
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
				   | BGFX_STATE_DEFAULT
		);
		
		bgfx::submit(RENDER_PASS_GEOMETRY,
					 m_shader->getHandle());
	}
	
	bgfx::DynamicVertexBufferHandle m_dvbh;
	bgfx::DynamicIndexBufferHandle m_dibh;
	Shader::Ptr         m_shader;

};

}

class AITModel {
public:

	void loadBody(char *buffer) {
		
		char *ptr = buffer;
		int idx = 0;
		
		// 2 bytes: flags
		int flags = (int16)READ_LE_UINT16(ptr + idx);

		// Jump 0xE(14)
		idx += 0xE;

		// Jump value returned + 2
		idx += (int16)READ_LE_UINT16(ptr + idx) + 2;

		// Num vertices
		num_vertices = *(int16 *)(ptr + idx);
		idx +=2;

		printf("Num. vertices: %d\n", num_vertices);

		vertices.clear();
		
		// float every 2 bytes
		for (int i = 0; i < num_vertices; i++) {

			float x =  *(int16 *)(ptr + idx + 0);
			float y =  *(int16 *)(ptr + idx + 2);
			float z =  *(int16 *)(ptr + idx + 4);

			vertices.push_back(Eigen::Vector3f(x, -y, z)/1000.0f);
			idx += 6;
		}

		// check bones
		if (flags & 2) {

			num_bones = *(int16 *)(ptr + idx);
			idx += 2;

			printf("Num. bones: %d\n", num_bones);
			
			// next num_bones * uint16_t are the offsets for the bone data
			idx += num_bones * 2;

			std::map<int, Bone> bone_map;
			
			// initialize bone map
			for (int b = 0; b < num_bones; b++) {
				bone_map[b] = Bone();
			}
			
			for (int b = 0; b < num_bones; b++) {

				int start_index = *(int16 *)(ptr + idx + 0) / 6;
				int num_points = *(int16 *)(ptr + idx + 2);
				int vertex_index = *(int16 *)(ptr + idx + 4) / 6;
				int parent_index = *(ptr + idx + 6);
				int bone_index = *(ptr + idx + 7);

				Bone* bone = &bone_map[bone_index];
				bone->parent = &bone_map[parent_index];
				bone->local_rot = Eigen::Quaternionf::Identity();
				bone->local_pos = vertices[vertex_index];

				//apply transform to vertices
				Eigen::Vector3f position = vertices[vertex_index];
				Eigen::Matrix<int, Eigen::Dynamic, 1> blending_weights =
					Eigen::Matrix<int, Eigen::Dynamic, 1>::Zero(num_points, 1);
				
				for (int u = 0; u < num_points; u++) {
					vertices[start_index] += position;
					//blending_weights[start_index] = bone_index;
					start_index++;
				}

				idx += 0x10;				
			}
		}
		else {
			//assert(false);
			printf("No bones\n");

		}
			
		// primitives
		
		int num_primitives =  *(int16 *)(ptr + idx);
		idx += 2;

		printf("Num. primitives: %d\n", num_primitives);
		
		for (int n = 0; n < num_primitives; n++) {
			
			int primitive_type = *(ptr + idx);
			idx++;
			
			switch(primitive_type) {
				
			case 0: //line
			{
				idx++;
				int color_index = *(ptr + idx);
				idx += 2;
				
				//Color32 color = getColorFromIndex(color_index);//paletteColors[color_index];
				
				int point_idxA = *(int16 *)(ptr + idx + 0) / 6;
				int point_idxB = *(int16 *)(ptr + idx + 2) / 6;
				
				idx += 4;

				Geometry::Primitive::Ptr primitive_ptr = Geometry::Primitive::Ptr(new Geometry::Line(vertices[point_idxA],
																	   vertices[point_idxB],
																	   color_index));
				primitives.push_back(primitive_ptr);					
				
				
				break;
			}
			case 1: //poly
			{
				int num_points_poly = *(ptr + idx);
				int poly_type = *(ptr + idx + 1);
				int color_index = *(ptr + idx + 2);
				idx += 3;

				std::vector<Vector3f> poly_points;
				
				for (int m = 0; m < num_points_poly; m++) {
					
					int point_index = *(int16 *)(ptr + idx) / 6;
					idx += 2;
					
					poly_points.push_back(vertices[point_index]);
				}

				Geometry::Primitive::Ptr primitive_ptr = Geometry::Primitive::Ptr(new Geometry::Polygon(poly_points, color_index));
				primitives.push_back(primitive_ptr);				
				
				break;
			}
			case 2: //point
			{
				break;
			}
			case 3: //sphere
			{
				idx++;
				int color_index = *(ptr + idx);
				idx +=2;
				int size = *(int16 *)(ptr + idx);
				idx +=2;
				int point_index = *(int16 *)(ptr + idx) / 6;
				idx +=2;
				
				Eigen::Vector3f position = vertices[point_index];
				
				
				break;
			}
			case 4:
			case 5:
			case 6:
			case 7:
				break;
			default:
				assert(false); //throw except.
			}				
		}
	}

	void generateMesh() {

		PosColorVertex::init();
		
		std::vector<PosColorVertex> vertices;
		std::vector<uint16_t> indices;
		int verticesCount = 0;
		
		for (auto prim : primitives) {

			verticesCount = vertices.size();
			
			if (prim->type == PRIM_TYPE_POLYGON) {
				auto poly_ptr = std::dynamic_pointer_cast<Geometry::Polygon>(prim);
				for (auto p : poly_ptr->points) {
					PosColorVertex pcv;
					pcv.m_x = p(0);
					pcv.m_y = p(1);
					pcv.m_z = p(2);
					pcv.m_abgr = 0;						
					vertices.push_back(pcv);
				}

				//triangulate
				int v0 = 0;
				int v1 = 1;
				int v2 = poly_ptr->points.size() - 1;
				bool swap = true;
				while (v1 < v2)
				{
					indices.push_back(verticesCount + v0);
					indices.push_back(verticesCount + v1);
					indices.push_back(verticesCount + v2);
					if (swap)
					{
						v0 = v1;
						v1++;
					}
					else
					{
						v0 = v2;
						v2--;
					}
					swap = !swap;
				}				
			}
		}

		mesh = std::make_shared<Geometry::Mesh>();
		const bgfx::Memory* mem;
		mem = bgfx::copy(&vertices[0], sizeof(PosColorVertex) * vertices.size() );
		mesh->m_dvbh = bgfx::createDynamicVertexBuffer(mem, PosColorVertex::ms_decl);
		
		mem = bgfx::copy(&indices[0], sizeof(uint16_t) * indices.size() );
		mesh->m_dibh = bgfx::createDynamicIndexBuffer(mem);

		mesh->m_shader = Shader::Ptr(new Shader("vs_cubes", "fs_cubes"));
		mesh->m_shader->init();
	}

	void draw() {

		mesh->submit();
		
	}
	
protected:

	int num_vertices = 0;
	int num_bones = 0;
	int num_primitives = 0;
	
	std::vector<Eigen::Vector3f> vertices;
	std::vector<Geometry::Primitive::Ptr> primitives;
	std::vector<Bone> bones;
	Geometry::Mesh::Ptr mesh;
	
	
};
