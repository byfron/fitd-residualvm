#pragma once

#include "Actor.hpp"
#include <hqr.h>

//TODO: send this to parseUtils ?
Eigen::Matrix3f getRotationMatrixFromRotIndices(uint16 rotx, uint16 roty, uint16 rotz);

class ActorLoader {
public:
	static Actor::Ptr load(int idx);
	static Animation::Ptr loadAnimation(Skeleton::Ptr skeleton, int anim_idx);
	
	static void cleanBuffers() { if (list_body) delete list_body; }
	static Fitd::hqrEntryStruct *list_body;
	static Fitd::hqrEntryStruct *list_anim;	
};
