#include "GameSystems.hpp"
#include "Components.hpp"

using namespace Components;

void InputSystem::update(EntityManager & em, EventManager &evm, float delta) {

	if (InputManager::m_keys & ACTOR_KEY_LEFT) {
		evm.emit<Msg::Move>(AITDEngine::player_entity, Vec3f(1.0, 0.0, 0.0));
		InputManager::setKeyState(ACTOR_KEY_LEFT, false);
	}
		
	if (InputManager::m_keys & ACTOR_KEY_RIGHT) {
		InputManager::setKeyState(ACTOR_KEY_RIGHT, false);
	}
	
	if (InputManager::m_keys & ACTOR_KEY_UP) {
		InputManager::setKeyState(ACTOR_KEY_UP, false);
	}
	
	if (InputManager::m_keys & ACTOR_KEY_DOWN) {
		InputManager::setKeyState(ACTOR_KEY_DOWN, false);
	}	
}

void UpdateSystem::receive(const Msg::Move &move) {

	//queue move components
	message_queue.push(Msg::Message::Ptr(new Msg::Move(move)));
}

void UpdateSystem::update(EntityManager & em, EventManager &evm, float delta ) {

	while(!message_queue.empty()) {

		std::shared_ptr<Msg::Move> msg = std::static_pointer_cast<Msg::Move>(message_queue.front());
		switch(msg->type) {
		case Msg::MOVE:
			TransformComponent *tc = em.getComponentPtr<TransformComponent>(msg->entity);
			tc->translation += msg->moveVector * delta;
			break;
		}

		message_queue.pop();
	}
	
	// em.each<TransformComponent>(
	// 	[delta](Entity entity,
	// 			TransformComponent &tc) {
	// 		//	tc.update(delta);
	// 	});		
	
}
