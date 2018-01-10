#include "RenderSystem.hpp"
#include "AITDEngine.hpp"
#include <Components.hpp>

using namespace Components;

// TODO: Render systems is super simple.
// Long time goal is to add lots of features (look at pumpkin)

void RenderSystem::update(EntityManager & em, EventManager &evm, float delta) {

	world->render(delta);	
	
	// Render all meshes
	em.each<MeshComponent, TransformComponent>(
		[delta](Entity entity,
				MeshComponent &mc,
				TransformComponent &tc) {
			
			//get transform			
			mc.render(delta, tc.getTransform().data());
		});

	
	// Render debug elements
	em.each<DebugComponent>(
		[delta](Entity entity,
				DebugComponent &dc) {
			dc.render(delta);
		});
}
