#pragma once

#include <filestream.h>
#include <hqr.h>
#include <map>

struct actorFlags {
	uint16 flag0x0001: 1;
	uint16 flag0x0002: 1;
	uint16 flag0x0004: 1;
	uint16 flag0x0008: 1;
	uint16 flag0x0010: 1;
	uint16 flag0x0020: 1;
	uint16 flag0x0040: 1;
	uint16 tackable: 1;
	uint16 flag0x0100: 1;
	uint16 flag0x0200: 1;
	uint16 flag0x0400: 1;
	uint16 flag0x0800: 1;
	uint16 flag0x1000: 1;
	uint16 flag0x2000: 1;
	uint16 flag0x4000: 1;
	uint16 flag0x8000: 1;
};

struct ObjectData {
	int16 ownerIdx; 
	int16 body;    // corresponding entry in actor (mesh) array 
	union {
		int16 flags;
		actorFlags bitField;
	};
	int16 field_6; //what?
	int16 foundBody;
	int16 foundName;
	int16 flags2;
	int16 foundLife; //index of the corresponding script entry? and life?
	int16 x;
	int16 y;
	int16 z;
	int16 alpha;
	int16 beta;
	int16 gamma;
	int16 stage;
	int16 room;
	int16 life_mode;
	int16 life;
	int16 field_24; //what?
	int16 anim; //index in the anim buffer
	int16 frame;
	int16 animType;
	int16 animInfo;
	int16 trackMode; //1: manual, 2: follow, 3: scripted
	int16 trackNumber;
	int16 positionInTrack;

	void readFromStream(Common::SeekableReadStream *stream);
};

class ObjectDataLoader {
public:

	ObjectDataLoader();
	Common::SeekableReadStream *stream;
	int num_objects;
	
};

class ObjectManager {
public:
	static void loadObjects();
	static std::map<int, ObjectData> object_map;
};
	
