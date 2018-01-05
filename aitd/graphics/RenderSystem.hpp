#pragma once

#include "GraphicsEngine.hpp"
#include <World.hpp>
#include <entities/Entity.hpp>
#include <entities/System.hpp>
#include <utils/Geometry.hpp>

class CameraComponent {
public:
	CameraComponent(const Eigen::Matrix4f& camera_view) {

		//initialize matrices
		memcpy(view, camera_view.data(), sizeof(float)*16);

		m_camera.init();
		
		//TODO query resolution from engine
		bx::mtxProj(proj, 60.0f, float(1280)/float(720), 0.1f, 100.0f,					
					bgfx::getCaps()->homogeneousDepth);		
	}

	void render(float delta) {

		m_camera.mtxLookAt(view);
		bgfx::setViewTransform(RENDER_PASS_GEOMETRY, view, proj);
		m_camera.update(delta);
	}

	float view[16];
	float proj[16];
	Camera m_camera;
};

class BgImageComponent {
public:
	BgImageComponent(unsigned char* buffer) {
		
		const bgfx::Memory* mem = bgfx::alloc(320*200*3);
		memcpy((unsigned char*)mem->data, buffer, 320*200*3);
		const uint32_t flags = 0
	 	| BGFX_TEXTURE_U_CLAMP
	 	| BGFX_TEXTURE_V_CLAMP
	 	| BGFX_TEXTURE_MIN_POINT
	 	| BGFX_TEXTURE_MAG_POINT;
		texture = bgfx::createTexture2D(320, 200, false, 1,
										bgfx::TextureFormat::RGB8, flags, mem);	
		program = Shader::Ptr(new Shader("vs_backg", "fs_backg"));
		program->init();
	}

	void render(float delta) {
		bgfx::setTexture(0, texture_uniform, texture);
		bgfx::setState(0
					   | BGFX_STATE_RGB_WRITE
					   | BGFX_STATE_ALPHA_WRITE);
		screenSpaceQuad( (float)1280, (float)720, false, 1.0f, 1.0f);
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

	MeshComponent(Geometry::Mesh::Ptr m) : mesh(m) {}
	
	void render(float delta) {
		mesh->submit();
	}
	
protected:
	
	Geometry::Mesh::Ptr mesh;
};

class RenderSystem : public System<RenderSystem> {
public:

	RenderSystem(World::Ptr w) : world(w) {}
	void update(EntityManager & em, EventManager &evm, float delta);

protected:

	World::Ptr world;
};
