#pragma once

#include "GraphicsEngine.hpp"
#include "AITModel.hpp"

void screenSpaceQuad(float _textureWidth, float _textureHeight, bool _originBottomLeft, float _width, float _height);

class ModelViewer : public GraphicsEngine {
public:

	ModelViewer(const char* name, const char* desc);

	~ModelViewer() {}

	void loadModel(int body_index);
	
	virtual void init_engine();

	void frame(const float dt) override {

		//paint background texture in background view		
		bgfx::setTexture(0, s_texUniform, test_tex);
		bgfx::setState(BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE);
		screenSpaceQuad( (float)m_width, (float)m_height, false, 1.0f, 1.0f);
		bgfx::submit(RENDER_PASS_BACKGROUND, bgProgram->getHandle());
		
		//model.draw();
	}

private:

	AITModel model;

	bgfx::TextureHandle test_tex;
	bgfx::UniformHandle s_texUniform;
	Shader::Ptr bgProgram;
	
};


