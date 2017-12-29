#pragma once

#include "GraphicsEngine.hpp"
#include "AITModel.hpp"

namespace Fitd {

struct hqrEntryStruct;
	
class ModelViewer : public pumpkin::GraphicsEngine {
public:

	ModelViewer(const char* name, const char* desc);

	~ModelViewer() {}

	void loadModel(int body_index);
	
	void init_engine() override {

	}

	void frame(const float dt) override {
		
	}

private:

	hqrEntryStruct *list_body;
};

}
