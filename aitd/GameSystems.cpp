#include "GameSystems.hpp"
#include "Components.hpp"
#include "AITDEngine.hpp"

using namespace Components;

void ScriptingSystem::update(EntityManager & em, EventManager &evm, float delta) {
	em.each<ScriptComponent>(
		[delta, this, em](Entity entity,
				ScriptComponent& sc) {
			scripting_manager->runScript(sc.script_id, entity.id(), em);
		});
}

void InputSystem::update(EntityManager & em, EventManager &evm, float delta) {

	em.each<MoveComponent, UserInputComponent>(
		[delta](Entity entity,
				MoveComponent& mc,
	 			UserInputComponent& tc) {

			mc.reset();
			
			if (InputManager::m_keys & ACTOR_KEY_LEFT) {
				float angle = mc.rotate_speed * delta;
				mc.rotation = Eigen::AngleAxisf(-angle, Eigen::Vector3f::UnitY());
				mc.move_vector = mc.rotation * mc.move_vector;				
				InputManager::setKeyState(ACTOR_KEY_LEFT, false);
			}
		
			if (InputManager::m_keys & ACTOR_KEY_RIGHT) {
				float angle = mc.rotate_speed * delta;
				mc.rotation = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitY());
				mc.move_vector = mc.rotation * mc.move_vector;				
				InputManager::setKeyState(ACTOR_KEY_RIGHT, false);
			}
	
			if (InputManager::m_keys & ACTOR_KEY_UP) {
				mc.translation = mc.move_vector * mc.move_speed * delta;
				InputManager::setKeyState(ACTOR_KEY_UP, false);
			}
	
			if (InputManager::m_keys & ACTOR_KEY_DOWN) {
				mc.translation = -mc.move_vector * mc.move_speed * delta;
				InputManager::setKeyState(ACTOR_KEY_DOWN, false);
			}	
		});
}

void UpdateSystem::update(EntityManager & em, EventManager &evm, float delta ) {
	
	// Update model transform with the move component
	em.each<MoveComponent, TransformComponent>(
		[delta](Entity entity,
				MoveComponent& mc,
	 			TransformComponent& tc) {
			tc.translation += mc.translation;
			tc.rotation *= mc.rotation;
	 	});			
}
