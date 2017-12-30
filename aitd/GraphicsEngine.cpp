#include "GraphicsEngine.hpp"
#include "ResourceManager.hpp"
#include <iostream>
#include <common/common.h>
	
Camera GraphicsEngine::m_camera;
bool GraphicsEngine::m_debug = false;
bgfx::VertexDecl PosColorVertex::ms_decl;
bgfx::VertexDecl PosTexCoordVertex::ms_decl;

float getDeltaTime() {
	int64_t now = bx::getHPCounter();
	static int64_t last = now;
	const int64_t frameTime = now - last;
	last = now;
	const double freq = double(bx::getHPFrequency() );
	return float(frameTime/freq);
}

void GraphicsEngine::start(int _argc, const char* const* _argv) {
	Args args(_argc, _argv);
	bgfx::init(args.m_type, args.m_pciId);

	m_width = 1280;
	m_height = 720;

	bgfx::reset(m_width, m_height, InputManager::m_reset);
	bgfx::setDebug(InputManager::m_debug);

	// Set palette color for index 0
	bgfx::setPaletteColor(0, UINT32_C(0x000000000) );

	// Set palette color for index 1
	bgfx::setPaletteColor(1, UINT32_C(0x303030ff) );

	bgfx::setViewClear(RENDER_PASS_BACKGROUND
			   , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH|BGFX_CLEAR_STENCIL
			   , 1.0f
			   , 0
			   , 1);
	
	bgfx::setViewClear(RENDER_PASS_GEOMETRY
			   , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH|BGFX_CLEAR_STENCIL
			   , 1.0f
			   , 0
			   , 1);

	initResources();

	init_engine();

	m_camera.init();
	m_input_manager.init();
}

void GraphicsEngine::initResources() {
	PosColorVertex::init();
	PosTexCoordVertex::init();
}

void GraphicsEngine::run() {

	const float deltaTime = getDeltaTime();

	// If the size of the window changes, update the size of framebuffers
	if (m_oldWidth  != m_width  ||  m_oldHeight != m_height) {
		m_oldWidth = m_width;
		m_oldHeight = m_height;

		const uint32_t samplerFlags = 0
			| BGFX_TEXTURE_RT
			| BGFX_TEXTURE_MIN_POINT
			| BGFX_TEXTURE_MAG_POINT
			| BGFX_TEXTURE_MIP_POINT
			| BGFX_TEXTURE_U_CLAMP
			| BGFX_TEXTURE_V_CLAMP;
	}

	// Default view
	bgfx::setViewRect(RENDER_PASS_BACKGROUND, 0, 0, m_width, m_height);
	bgfx::setViewRect(RENDER_PASS_GEOMETRY, 0, 0, m_width, m_height);

	// Set geometry view
	float view[16];
	float proj[16];


	// draw texture in background
	bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
	bgfx::setViewTransform(RENDER_PASS_BACKGROUND, NULL, proj);
	bgfx::setViewRect(RENDER_PASS_BACKGROUND, 0, 0, m_width, m_height);

	
	// m_camera.mtxLookAt(view);
	// bx::mtxProj(proj, 60.0f, float(m_width)/float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
	// bgfx::setViewTransform(RENDER_PASS_GEOMETRY, view, proj);
	// bgfx::setViewRect(RENDER_PASS_GEOMETRY, 0, 0, uint16_t(m_width), uint16_t(m_height) );
					
	bgfx::touch(0);

	frame(deltaTime);

	bgfx::frame();

	m_camera.update(deltaTime);
}

void GraphicsEngine::stop() {

}
