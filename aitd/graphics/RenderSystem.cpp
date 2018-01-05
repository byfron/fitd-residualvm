#include "RenderSystem.hpp"
#include "AITDEngine.hpp"

void RenderSystem::update(EntityManager & em, EventManager &evm, float delta ) {

	world->render(delta);	

	// Render debug elements
	em.each<DebugComponent>(
		[delta](Entity entity,
				DebugComponent &dc) {
			dc.render(delta);
		});		
	
	// Render all meshes
	em.each<MeshComponent>(
		[delta](Entity entity,
				MeshComponent &mc) {
			mc.render(delta);
		});
}
