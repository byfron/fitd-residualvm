#include "ModelViewer.hpp"
#include "AITDDataLoader.hpp"
#include "Floor.hpp"

#include <hqr.h>
#include <fitd.h>
#include <main.h>

using namespace Fitd;



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

	
	RoomCamera::Ptr room_cam = floor.getCamera(0);
	const bgfx::Memory* mem = bgfx::alloc(320*200*3);
	memcpy((unsigned char*)mem->data, room_cam->getBackgroundImagePtr(), 320*200*3);

	const uint32_t flags = 0
		| BGFX_TEXTURE_U_CLAMP
		| BGFX_TEXTURE_V_CLAMP
		| BGFX_TEXTURE_MIN_POINT
		| BGFX_TEXTURE_MAG_POINT;

	test_tex = bgfx::createTexture2D(320, 200, false, 1, bgfx::TextureFormat::RGB8, flags, mem);
	
	bgProgram = Shader::Ptr(new Shader("vs_backg", "fs_backg"));
	bgProgram->init();
}

void ModelViewer::loadModel(int body_index) {

}
