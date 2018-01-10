#include "ObjectManager.hpp"
#include <resource.h>
#include <file_access.h>
#include <iostream> //TODO: Change all those outputs with a proper logger

std::map<int, ObjectData> ObjectManager::object_map;

void ObjectData::readFromStream(Common::SeekableReadStream *stream) {
	this->ownerIdx = stream->readUint16LE(); //OwnerIdx is ALWAYS -1 at loading!!
	this->body = stream->readUint16LE();
	this->flags = stream->readUint16LE();
	this->field_6 = stream->readUint16LE();
	this->foundBody = stream->readUint16LE();
	this->foundName = stream->readUint16LE();
	this->flags2 = stream->readUint16LE();
	this->foundLife = stream->readUint16LE();
	this->x = stream->readUint16LE();
	this->y = stream->readUint16LE();
	this->z = stream->readUint16LE();
	this->alpha = stream->readUint16LE();
	this->beta = stream->readUint16LE();
	this->gamma = stream->readUint16LE();
	this->stage = stream->readUint16LE();
	this->room = stream->readUint16LE();
	this->lifeMode = stream->readUint16LE();
	this->life = stream->readUint16LE();	
	this->field_24 = stream->readUint16LE();
	this->anim = stream->readUint16LE();
	this->frame = stream->readUint16LE();
	this->animType = stream->readUint16LE();
	this->animInfo = stream->readUint16LE();
	this->trackMode = stream->readUint16LE();
	this->trackNumber = stream->readUint16LE();
	this->positionInTrack = stream->readUint16LE();
	this->flags |= 0x20;
}

ObjectDataLoader::ObjectDataLoader() {
	stream = Fitd::g_resourceLoader->getFile("OBJETS.ITD");
	if(!stream)
		Fitd::theEnd(0, "OBJETS.ITD");
	num_objects = stream->readUint16LE();
}

void ObjectManager::loadObjects() {

	ObjectDataLoader data_loader;
	for(int i = 0; i < data_loader.num_objects; i++) {
		object_map[i].readFromStream(data_loader.stream);
	}
	delete data_loader.stream;
}
