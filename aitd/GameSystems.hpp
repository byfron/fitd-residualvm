#pragma once

#include <entities/System.hpp>
#include "World.hpp"
#include "InputManager.hpp"
#include "Message.hpp"
#include <queue>

class InputSystem : public System<InputSystem> {
public:

	void update(EntityManager& em, EventManager &evm, float delta);
};

class UpdateSystem : public System<UpdateSystem>,
					 public Receiver<UpdateSystem> {
public:	
	UpdateSystem(World::Ptr w) : world(w) {}
	void update(EntityManager& em, EventManager &evm, float delta);
	void receive(const Msg::Move &move);
	
	protected:

	std::queue<Msg::Message::Ptr> message_queue;
	World::Ptr world;
};
