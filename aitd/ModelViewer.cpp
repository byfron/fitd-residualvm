#include "ModelViewer.hpp"
#include "AITDDataLoader.hpp"
#include "Floor.hpp"

#include <hqr.h>
#include <fitd.h>
#include <main.h>

using namespace Fitd;

//TODO: put in utils
void screenSpaceQuad(float _textureWidth, float _textureHeight, bool _originBottomLeft = false, float _width = 1.0f, float _height = 1.0f)
{
	if (3 == bgfx::getAvailTransientVertexBuffer(3, PosTexCoordVertex::ms_decl) )
	{
		bgfx::TransientVertexBuffer vb;
		bgfx::allocTransientVertexBuffer(&vb, 3, PosTexCoordVertex::ms_decl);
		PosTexCoordVertex* vertex = (PosTexCoordVertex*)vb.data;

		const float zz = 0.0f;

		const float minx = -_width;
		const float maxx =  _width;
		const float miny = 0.0f;
		const float maxy = _height*2.0f;

		const bgfx::Caps* caps = bgfx::getCaps();
		const float s_texelHalf = bgfx::RendererType::Direct3D9 == caps->rendererType ? 0.5f : 0.0f;
		const float texelHalfW = s_texelHalf/_textureWidth;
		const float texelHalfH = s_texelHalf/_textureHeight;
		const float minu = -1.0f + texelHalfW;
		const float maxu =  1.0f + texelHalfW;

		float minv = texelHalfH;
		float maxv = 2.0f + texelHalfH;

		if (_originBottomLeft)
		{
			float temp = minv;
			minv = maxv;
			maxv = temp;

			minv -= 1.0f;
			maxv -= 1.0f;
		}

		vertex[0].m_x = minx;
		vertex[0].m_y = miny;
		vertex[0].m_z = zz;
		vertex[0].m_u = minu;
		vertex[0].m_v = minv;

		vertex[1].m_x = maxx;
		vertex[1].m_y = miny;
		vertex[1].m_z = zz;
		vertex[1].m_u = maxu;
		vertex[1].m_v = minv;

		vertex[2].m_x = maxx;
		vertex[2].m_y = maxy;
		vertex[2].m_z = zz;
		vertex[2].m_u = maxu;
		vertex[2].m_v = maxv;

		bgfx::setVertexBuffer(0, &vb);
	}
}



ModelViewer::ModelViewer(const char* name, const char* desc) : GraphicsEngine(name, desc) {


}

void ModelViewer::init_engine() {

	// Maybe eventually we can get rid of this part
 	Fitd::init();

	AITDDataLoader::loadColorPalette();
	
 	hqrEntryStruct *list_body;
 	list_body = new Fitd::hqrEntryStruct("LISTBODY", 100000, 50); // was calculated from free mem size
 	char *ptr = list_body->get(45);	
 	model.loadBody(ptr);
 	model.generateMesh();

	Floor floor;
	floor.load(0);

	test_tex = loadTexture("textures/test.dds"
				, 0
				| BGFX_TEXTURE_U_CLAMP
				| BGFX_TEXTURE_V_CLAMP
				| BGFX_TEXTURE_W_CLAMP
				);
	
	bgProgram = Shader::Ptr(new Shader("vs_backg", "fs_backg"));
	bgProgram->init();
}

void ModelViewer::loadModel(int body_index) {

}
