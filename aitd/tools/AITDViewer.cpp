#include "AITDViewer.hpp"
#include <utils/DataParsing.hpp>
#include <utils/Color.hpp>
#include <graphics/RenderSystem.hpp>


#include <main.h>

AITDViewer::~AITDViewer() {
	ActorLoader::cleanBuffers();
}


void AITDViewer::init() {

	Fitd::init(false);
	DataParsing::init();
	ColorPalette::loadPalette();

	entity_manager = EntityManager::Ptr(new EntityManager());
	
	//create actor entity
	int actor_idx = 45;
	actor_data = ActorLoader::load(actor_idx);
	Entity actor = entity_manager->createLocal();
	entity_manager->assign<MeshComponent>(actor.id(), actor_data->getMesh());
}

void AITDViewer::run_frame(float dt) {

	float view[16];
	float proj[16];
	bx::mtxProj(proj, 60.0f, float(GraphicsEngine::WIDTH)/float(GraphicsEngine::HEIGHT),
				0.1f, 1000000.0f, bgfx::getCaps()->homogeneousDepth);
	GraphicsEngine::camera().mtxLookAt(view);
	
	bgfx::setViewTransform(RENDER_PASS_GEOMETRY, view, proj);
	
	// Render all meshes
	entity_manager->each<MeshComponent>(
		[dt](Entity entity,
				MeshComponent &mc) {
			mc.render(dt);
		});

	// Render debug elements
	entity_manager->each<DebugComponent>(
		[dt](Entity entity,
				DebugComponent &dc) {
			dc.render(dt);
		});
}
