#include "ModelViewer.hpp"

#include <hqr.h>
#include <fitd.h>
#include <main.h>

using namespace Fitd;

ModelViewer::ModelViewer(const char* name, const char* desc) : GraphicsEngine(name, desc) {


}

void ModelViewer::init_engine() {
	
 	Fitd::init();
 	hqrEntryStruct *list_body;
 	list_body = new Fitd::hqrEntryStruct("LISTBODY", 100000, 50); // was calculated from free mem size
 	char *ptr = list_body->get(15);	
 	model.loadBody(ptr);
 	model.generateMesh();
}

void ModelViewer::loadModel(int body_index) {
}

