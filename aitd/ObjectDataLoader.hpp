#pragma once

struct ObjectData {
	int16 ownerIdx; //actor related to this object_data
	int16 body;
	union {
		int16 flags;
		actorFlags bitField;
	};
	int16 field_6; //what?
	int16 foundBody;
	int16 foundName;
	int16 flags2;
	int16 foundLife;
	int16 x;
	int16 y;
	int16 z;
	int16 alpha;
	int16 beta;
	int16 gamma;
	int16 stage;
	int16 room;
	int16 lifeMode;
	int16 life;
	int16 field_24; //what?
	int16 anim;
	int16 frame;
	int16 animType;
	int16 animInfo;
	int16 trackMode;
	int16 trackNumber;
	int16 positionInTrack;

	void readFromStream(Common::SeekableReadStream *stream);
};

class ObjectDataLoader {
public:

	ObjectDataLoader();
	~ObjectDataLoader();

protected:
	Common::SeekableReadStream *stream;

	
};
