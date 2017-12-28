#pragma once

#include "GraphicsEngine.hpp"
#include <spdlog/spdlog.h>

namespace spd = spdlog;

class ModelViewer : public pumpkin::GraphicsEngine {
public:

	ModelViewer(const char* name, const char* desc) : GraphicsEngine(name, desc) {
		auto console = spd::stdout_logger_mt("Client");
	}

	~ModelViewer() {}

	void init_engine() override {

	}

	void frame(const float dt) override {
		
	}

private:

};
