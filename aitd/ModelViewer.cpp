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
	
	
}

void ModelViewer::loadModel(int body_index) {
}

