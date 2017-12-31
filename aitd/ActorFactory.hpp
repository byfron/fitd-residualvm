#pragma once

#include "Actor.hpp"
#include <hqr.h>

class ActorFactory {
public:

	static Actor::Ptr create(int idx);
	static void cleanBuffers() { if (list_body) delete list_body; }
	static Fitd::hqrEntryStruct *list_body;
};
