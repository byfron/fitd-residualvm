#pragma once

#include <entities/System.hpp>
#include "World.hpp"

class UpdateSystem : public System<UpdateSystem> {
	public:
	UpdateSystem(World::Ptr w) : world(w) {}
	void update(EntityManager & em, EventManager &evm, float delta);		

	protected:

	World::Ptr world;
};
