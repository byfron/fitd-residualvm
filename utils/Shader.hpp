#pragma once

#include "ResourceManager.hpp"
#include <common/common.h>
#include <string>
#include <iostream>
#include <memory>
#include <common/bgfx_utils.h>
//#include <shader.pb.h>

class Shader : public Resource{
public:

	friend class ShaderFactory;
	typedef std::shared_ptr<Shader> Ptr;

//	Shader(const voyage::ShaderCfg & cfg);
	Shader(std::string fs,
	       std::string vs) : m_vs_shader(vs), m_fs_shader(fs) {
	}

	~Shader() {
//		bgfx::destroyProgram(m_program);
	}

	static uint32_t type() { return 1; }

	void init();

	bgfx::ProgramHandle & getHandle() {
		assert(m_initialized);
		return m_program;
	}

protected:

	bool m_initialized = false;
	std::string m_vs_shader;
	std::string m_fs_shader;
	bgfx::ProgramHandle m_program;
};
