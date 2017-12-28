#include "Shader.hpp"
//#include "ShaderFactory.hpp"
#include <bx/readerwriter.h>

namespace pumpkin {

//////////////// Replicates shader loading utils with custom paths //////////////

const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath)
{
	if (bx::open(_reader, _filePath) )
	{
		uint32_t size = (uint32_t)bx::getSize(_reader);
		const bgfx::Memory* mem = bgfx::alloc(size+1);
		bx::read(_reader, mem->data, size);
		bx::close(_reader);
		mem->data[mem->size-1] = '\0';
		return mem;
	}

	DBG("Failed to load %s.", _filePath);
	return NULL;
}

bgfx::ShaderHandle pumpkinLoadShader(bx::FileReaderI* _reader, const char* _name)
{
	char filePath[512];

	std::string shaderPath = "";//SHADER_FILE_PATH;

	switch (bgfx::getRendererType() )
	{
	case bgfx::RendererType::Noop:
	case bgfx::RendererType::Direct3D9:  shaderPath += "dx9/";   break;
	case bgfx::RendererType::Direct3D11:
	case bgfx::RendererType::Direct3D12: shaderPath += "dx11/";  break;
	case bgfx::RendererType::Gnm:        shaderPath += "pssl/";  break;
	case bgfx::RendererType::Metal:      shaderPath += "metal/"; break;
	case bgfx::RendererType::OpenGL:     shaderPath += "glsl/";  break;
	case bgfx::RendererType::OpenGLES:   shaderPath += "essl/";  break;
	case bgfx::RendererType::Vulkan:     shaderPath += "spirv/"; break;

	case bgfx::RendererType::Count:
		BX_CHECK(false, "You should not be here!");
		break;
	}

	strcpy(filePath, shaderPath.c_str());
	strcat(filePath, _name);
	strcat(filePath, ".bin");

	return bgfx::createShader(loadMem(_reader, filePath) );
}

bgfx::ShaderHandle pumpkinLoadShader(const char* _name)
{
	return pumpkinLoadShader(entry::getFileReader(), _name);
}

bgfx::ProgramHandle pumpkinLoadProgram(bx::FileReaderI* _reader, const char* _vsName, const char* _fsName)
{
	bgfx::ShaderHandle vsh = pumpkinLoadShader(_reader, _vsName);
	bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
	if (NULL != _fsName)
	{
		fsh = pumpkinLoadShader(_reader, _fsName);
	}

	return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
}

bgfx::ProgramHandle pumpkinLoadProgram(const char* _vsName, const char* _fsName)
{
	return pumpkinLoadProgram(entry::getFileReader(), _vsName, _fsName);
}

////////////////////////////////////////////////////////////////////////////////
//Shader::Shader(const voyage::ShaderCfg & cfg) {
//	ShaderFactory factory(cfg);
//	factory.generate(this);
//}


void Shader::init() {
	m_program = pumpkinLoadProgram(m_vs_shader.c_str(), m_fs_shader.c_str());
	m_initialized = true;
}

};
