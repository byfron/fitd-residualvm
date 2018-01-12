#pragma once

#include "GraphicsEngine.hpp"
#include <World.hpp>
#include <Actor.hpp>
#include <entities/Entity.hpp>
#include <entities/System.hpp>
#include <utils/Geometry.hpp>

class CameraComponent {
public:
	CameraComponent(const Eigen::Matrix4f& camera_proj, const Eigen::Matrix4f& camera_view) {

		//initialize matrices
		memcpy(view, camera_view.data(), sizeof(float)*16);
		memcpy(proj, camera_proj.data(), sizeof(float)*16);		
		m_camera.init();
	}

	void render(float delta) {

		//m_camera.mtxLookAt(view);
		bgfx::setViewTransform(RENDER_PASS_GEOMETRY, view, proj);
//		m_camera.update(delta);
	}

	float view[16];
	float proj[16];
	Camera m_camera;
};

class BgImageComponent {
public:
	BgImageComponent(unsigned char* buffer) {

		const int num_pixels = GraphicsEngine::WIDTH * GraphicsEngine::HEIGHT;
		const bgfx::Memory* mem = bgfx::alloc(num_pixels*3);
		memcpy((unsigned char*)mem->data, buffer, num_pixels*3);
		const uint32_t flags = 0
	 	| BGFX_TEXTURE_U_CLAMP
	 	| BGFX_TEXTURE_V_CLAMP
	 	| BGFX_TEXTURE_MIN_POINT
	 	| BGFX_TEXTURE_MAG_POINT;
		texture = bgfx::createTexture2D(GraphicsEngine::WIDTH, GraphicsEngine::HEIGHT, false, 1,
										bgfx::TextureFormat::RGB8, flags, mem);	
		program = Shader::Ptr(new Shader("vs_backg", "fs_backg"));
		program->init();
	}

	void render(float delta) {
		bgfx::setTexture(0, texture_uniform, texture);
		bgfx::setState(0
					   | BGFX_STATE_RGB_WRITE
					   | BGFX_STATE_ALPHA_WRITE);
		screenSpaceQuad((float)GraphicsEngine::WIDTH, (float)GraphicsEngine::HEIGHT, false, 1.0f, 1.0f);
		bgfx::submit(RENDER_PASS_BACKGROUND, program->getHandle());
	}
	
protected:

	Shader::Ptr program;
	bgfx::TextureHandle texture;
	bgfx::UniformHandle texture_uniform;
};

class DebugComponent {
public:
	DebugComponent(Geometry::DebugMesh::Ptr d) : debug_mesh(d) {}

	void render(float delta) {	
		debug_mesh->render(delta);
	}
protected:

	Geometry::DebugMesh::Ptr debug_mesh;
};

class MeshComponent {

public:

	MeshComponent(Actor::Ptr actor) {
		offsets = actor->offsets;
		vertices = actor->vertices;
		primitives = actor->primitives;
		skeleton = actor->skeleton->deepCopy();
		generateMesh();		
	}

	//TODO: put this in the system to have components contain no logic at all
	void render(float delta, const float* mtx) {
		bgfx::setTransform(mtx);
		mesh->submit();
	}

	void generateMesh();
	void updateVertices();
	
	void updateSkeleton(Skeleton::Ptr skel) {
		skeleton = skel;
		updateVertices();
	}
	
protected:
	
	//mesh for rendering
	Geometry::Mesh::Ptr mesh;

	//offsets from bone locations. Fixed since loading time.
	std::vector<Eigen::Vector3f> offsets; 

	//includes vertices and bones. Is modified when the mesh animates
	std::vector<Eigen::Vector3f> vertices; 
	std::vector<uint16_t> indices;
	
	//vector of polygons pointing to the vertex buffer
	std::vector<Primitive::Ptr> primitives;

	//skeleton that defines the mesh deformation
	Skeleton::Ptr skeleton;

};

class RenderSystem : public System<RenderSystem> {
public:

	RenderSystem(World::Ptr w) : world(w) {}
	void update(EntityManager & em, EventManager &evm, float delta);

protected:

	World::Ptr world;
};
