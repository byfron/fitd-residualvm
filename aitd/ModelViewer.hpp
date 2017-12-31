#pragma once

#include "GraphicsEngine.hpp"
#include "AITModel.hpp"

class ModelViewer : public GraphicsEngine {
public:

	ModelViewer(const char* name, const char* desc);

	~ModelViewer() {}

	void loadModel(int body_index);
	
	virtual void init_engine();


	void frame(const float dt) override {

		//TODO
		aitd_engine->draw();
		
	
		
		model.draw();
	}

private:

	AITDEngine aitd_engine;
	AITModel model;

	
	
};


