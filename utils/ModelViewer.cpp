#include "ModelViewer.hpp"
#include <hqr.h>

namespace Fitd {
ModelViewer::ModelViewer(const char* name, const char* desc) : GraphicsEngine(name, desc) {
	
	
	list_body = new Fitd::hqrEntryStruct("LISTBODY", 100000, 50); // was calculated from free mem size
	
}

void ModelViewer::loadModel(int body_index) {
	char *ptr = list_body->get(body_index);
	AITModel model;
	model.loadBody(ptr);
}
	
}
