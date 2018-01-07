#ifndef ENGINE_H_HEADER_GUARD
#define ENGINE_H_HEADER_GUARD

#include <bgfx/bgfx.h>
#include <common/bgfx_utils.h>
#include <common/common.h>
#include "InputManager.hpp"
#include "Shader.hpp"
#include <bx/bx.h>
#include <bx/macros.h>

class Camera;

void screenSpaceQuad(float _textureWidth, float _textureHeight, bool _originBottomLeft,
					 float _width, float _height);

struct PosTexCoordVertex
{
	float m_x;
	float m_y;
	float m_z;
	float m_u;
	float m_v;


	static void init()
	{
		ms_decl
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();
	};

	static bgfx::VertexDecl ms_decl;
};

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


// Views
enum RenderPasses {
	
	RENDER_PASS_BACKGROUND = 0,
	RENDER_PASS_GEOMETRY
};


struct RenderState
{
	uint64_t m_state;
	uint32_t m_blendFactorRgba;
	uint32_t m_fstencil;
	uint32_t m_bstencil;
};


class GraphicsEngine : public entry::AppI {

public:

	GraphicsEngine(const char*name, const char*desc)
		: entry::AppI(name, desc) {
	}

	virtual ~GraphicsEngine() {}
	
	void init(int32_t _argc, const char* const* _argv, uint32_t width, uint32_t height) {
		start(_argc, _argv);
	}

	virtual int shutdown() override {
		stop();
	}

	bool update() override
	{

		if (!InputManager::processEvents(m_width, m_height)) {
			run();

			//submit frame
			bgfx::frame();

			return true;
		}

		return false;
	}

	void initResources();

	virtual void frame(const float) {};
	virtual void init_engine() = 0;

	void run();
	void start(int _argc, const char* const* _argv);
	void stop();


	static Camera & camera() { return m_camera; }
	static bool debugEnabled() { return m_debug; }

	static int WIDTH;
	static int HEIGHT;
protected:

	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_oldWidth = 0;
	uint32_t m_oldHeight = 0;
	static Camera m_camera;
	static bool m_debug;
	

	InputManager m_input_manager;

};


#endif
