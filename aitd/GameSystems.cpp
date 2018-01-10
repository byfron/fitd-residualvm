#include "GameSystems.hpp"
#include "Components.hpp"

using namespace Components;

void UpdateSystem::update(EntityManager & em, EventManager &evm, float delta ) {

	em.each<TransformComponent>(
		[delta](Entity entity,
				TransformComponent &tc) {
			//	tc.update(delta);
		});		
	
}
