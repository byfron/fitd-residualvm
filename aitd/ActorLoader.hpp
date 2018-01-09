#pragma once

#include "Actor.hpp"
#include <hqr.h>

class ActorLoader {
public:
	static Actor::Ptr load(int idx);
	static void loadAnimation(Actor::Ptr actor, int anim_idx, int frame);
	static void cleanBuffers() { if (list_body) delete list_body; }
	static Fitd::hqrEntryStruct *list_body;
	static Fitd::hqrEntryStruct *list_anim;	
};
