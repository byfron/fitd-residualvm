#include "GameSystems.hpp"
#include "Components.hpp"
#include "AITDEngine.hpp"

using namespace Components;

void ScriptingSystem::update(EntityManager & em, EventManager &evm, float delta) {
	// em.each<ScriptComponent>(
	// 	[delta, this, em](Entity entity,
	// 			ScriptComponent& sc) {
	// 		scripting_manager->runScript(sc.script_id, entity.id(), em);
	// 	});
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

void CollisionSystem::update(EntityManager & em, EventManager &evm, float delta ) {

	std::vector<std::pair<Entity::Id, SceneCollisionComponent> > scene_collisions;
	std::vector<std::pair<Entity::Id, ActorCollisionComponent> > actor_collisions;
	
	// get all scene and actor collision elements
	em.each<SceneCollisionComponent>(
		[&scene_collisions](Entity entity,
							SceneCollisionComponent& cc) {			
			cc.reset();
			scene_collisions.push_back({entity.id(), cc});
	 	});

	em.each<ActorCollisionComponent>(
		[&actor_collisions](Entity entity,
							ActorCollisionComponent& ac) {			
			ac.reset();
			actor_collisions.push_back({entity.id(), ac});
	 	});

	//correct move component to account for hard collisions
	em.each<MoveComponent, ActorCollisionComponent>(
		[delta, scene_collisions, actor_collisions](Entity entity,
				MoveComponent& mc,
	 			ActorCollisionComponent& cc) {

			ActorCollisionComponent this_col = cc;
			this_col.bounding_box.translation += mc.translation;
			
			//check collision with scene
			for (auto& sc : scene_collisions) {
				if (sc.second.checkCollision(&this_col)) {

					std::cout << "colliding with scene:" << sc.first.id << std::endl;
					//color shape colliding

					
					// Vec3f corrected_v = Geometry::computeVectorToCollision(cc.bounding_box,
					// 													   sc.second.bounding_box,
					// 													   mc.translation);
					// mc.translation = corrected_v;
				}
			}

			for (auto& ac : actor_collisions) {

				//avoid self-collision
				if (ac.first.id == entity.id().id)
					continue;
				
				if (ac.second.checkCollision(&this_col)) {
					std::cout << "colliding with " << ac.first.id << "!!" << std::endl;

//					if(actorTouchedPtr->flags & 0x10) //can be pushed

					
				}
			}
			
	 	});
}

void UpdateSystem::update(EntityManager & em, EventManager &evm, float delta ) {

	// Update collision components (TODO better... ue the updated transform to transform box)
	em.each<MoveComponent, ActorCollisionComponent>(
		[delta](Entity entity,
				MoveComponent& mc,
	 			ActorCollisionComponent& ac) {
			ac.bounding_box.translation += mc.translation;
	 	});			
	
	// Update model transform with the move component
	em.each<MoveComponent, TransformComponent>(
		[delta](Entity entity,
				MoveComponent& mc,
	 			TransformComponent& tc) {
			tc.translation += mc.translation;
			tc.rotation *= mc.rotation;
	 	});
}
