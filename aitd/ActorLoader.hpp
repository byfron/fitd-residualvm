#pragma once

#include "Actor.hpp"
#include <hqr.h>

class ActorLoader {
public:

	static Actor::Ptr load(int idx);
	static void cleanBuffers() { if (list_body) delete list_body; }
	static Fitd::hqrEntryStruct *list_body;
};
