#pragma once

#include <World.hpp>
#include <entities/Entity.hpp>
#include <entities/System.hpp>
#include <utils/Geometry.hpp>

class BgImageComponent {
public:
	BgImageComponent(unsigned char* buffer) {
		// const bgfx::Memory* mem = bgfx::alloc(320*200*3);
		// memcpy((unsigned char*)mem->data, buffer, 320*200*3);
		// const uint32_t flags = 0
	 	// | BGFX_TEXTURE_U_CLAMP
	 	// | BGFX_TEXTURE_V_CLAMP
	 	// | BGFX_TEXTURE_MIN_POINT
	 	// | BGFX_TEXTURE_MAG_POINT;
		// test_tex = bgfx::createTexture2D(320, 200, false, 1,
		// 								 bgfx::TextureFormat::RGB8, flags, mem);	
		// bg_program = Shader::Ptr(new Shader("vs_backg", "fs_backg"));
		// bg_program->init();
	}
protected:
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
