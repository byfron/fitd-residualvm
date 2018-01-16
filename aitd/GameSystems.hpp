#pragma once

#include <entities/System.hpp>
#include "World.hpp"
#include "InputManager.hpp"
#include "ScriptingManager.hpp"
#include "Message.hpp"
#include <queue>

class ScriptingSystem : public System<ScriptingSystem> {
public:
	ScriptingSystem(ScriptingManager::Ptr sm) : scripting_manager(sm) {}
	void update(EntityManager& em, EventManager &evm, float delta);
protected:   
	ScriptingManager::Ptr scripting_manager;	
};

class InputSystem : public System<InputSystem> {
public:

	void update(EntityManager& em, EventManager &evm, float delta);
};

class UpdateSystem : public System<UpdateSystem> {
public:	
	UpdateSystem(World::Ptr w) : world(w) {}
	void update(EntityManager& em, EventManager &evm, float delta);
	
protected:

	World::Ptr world;
};
