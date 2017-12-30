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
	
// Views
enum RenderPasses {
	RENDER_PASS_VISIBILITY = 0,
	RENDER_PASS_GEOMETRY,
	RENDER_PASS_POSTPROCESS
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


	void  screenSpaceQuad(float _textureWidth, float _textureHeight, float _texelHalf, bool _originBottomLeft, float _width, float _height);
	static Camera & camera() { return m_camera; }
	static bool debugEnabled() { return m_debug; }
protected:

	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_oldWidth = 0;
	uint32_t m_oldHeight = 0;
	static Camera m_camera;
	static bool m_debug;

	InputManager m_input_manager;

	// Refactor this in a PostProcessor
	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle m_ibh;


	bgfx::ProgramHandle m_program;
};


#endif
