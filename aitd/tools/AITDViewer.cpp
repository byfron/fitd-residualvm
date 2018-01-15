#include "AITDViewer.hpp"
#include "Components.hpp"
#include <utils/DataParsing.hpp>
#include <utils/Color.hpp>
#include <graphics/RenderSystem.hpp>
#include <main.h>

void display_vertices_of_bone(int bone_id, Actor::Ptr actor_data, EntityManager::Ptr entity_manager) {

	Bone::Ptr bone = actor_data->getSkeleton()->bone_map[bone_id];
	if (!bone) return;
	
	int start_v = bone->start_vertex_index;
	
	for (int i = 0; i < bone->num_vertices_affected; i++) {
		Vec3f pos = actor_data->getVertices()[start_v + i];
		Entity vertex = entity_manager->createLocal();
		entity_manager->assign<DebugComponent>(
			vertex.id(),
			Geometry::DebugMesh::Ptr(new Geometry::DebugSphere(pos, 15)));
	}
}

AITDViewer::~AITDViewer() {
	ActorLoader::cleanBuffers();
}

void AITDViewer::init() {

	Fitd::init(false);
	DataParsing::init();
	ColorPalette::loadPalette();

	entity_manager = EntityManager::Ptr(new EntityManager());
	
	//create actor entity
//	for (int actor_idx = 0; actor_idx < 60; actor_idx++)

	Actor::Ptr actor_data = ActorLoader::load(12);
	int anim_id = 5;
	Animation::Ptr anim_data = ActorLoader::loadAnimation(actor_data->skeleton, anim_id);
	
	Entity actor = entity_manager->createLocal();
	entity_manager->assign<MeshComponent>(actor.id(), actor_data);
	entity_manager->assign<Components::AnimationComponent>(actor.id(), anim_id, anim_data);
}

void AITDViewer::run_frame(float dt) {

	float view[16];
	float proj[16];
	bx::mtxProj(proj, 60.0f, float(GraphicsEngine::WIDTH)/float(GraphicsEngine::HEIGHT),
				0.1f, 1000000.0f, bgfx::getCaps()->homogeneousDepth);
	GraphicsEngine::camera().mtxLookAt(view);
	
	bgfx::setViewTransform(RENDER_PASS_GEOMETRY, view, proj);

	static Eigen::Matrix4f t = Eigen::Matrix4f::Identity();

	////// TODO: this is duplicated in RenderSystem ....
	
	// Update model mesh with current animation
	entity_manager->each<MeshComponent, Components::AnimationComponent>(
		[dt](Entity entity,
			 MeshComponent &mc,
			 Components::AnimationComponent &ac) {
			
			// perform interpolation			
			Skeleton::Ptr skel = ac.getInterpolatedSkeleton();
			
			// update mesh bones
			mc.updateSkeleton(skel);
		});
	
	// Render all meshes
	entity_manager->each<MeshComponent>(
		[dt](Entity entity,
				MeshComponent &mc) {
			mc.render(dt, t.data());
		});

	// Render debug elements
	entity_manager->each<DebugComponent>(
		[dt](Entity entity,
				DebugComponent &dc) {
			dc.render(dt);
		});

	// handy to rotate objects around
	GraphicsEngine::camera().update(dt);
}
