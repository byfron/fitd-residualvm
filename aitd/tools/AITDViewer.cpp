#include "AITDViewer.hpp"
#include <utils/DataParsing.hpp>
#include <utils/Color.hpp>
#include <graphics/RenderSystem.hpp>
#include <main.h>

void display_vertices_of_bone(int bone_id, Actor::Ptr actor_data, EntityManager::Ptr entity_manager) {

	Bone::Ptr bone = actor_data->getSkeleton()->bone_map[bone_id];
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

	actor_data = ActorLoader::load(12);
	
	Entity actor = entity_manager->createLocal();
	entity_manager->assign<MeshComponent>(actor.id(), actor_data->getMesh());

	//mark with a sphere the locations of bones
	for (auto bone_it : actor_data->getSkeleton()->bone_map) {
		Entity bone = entity_manager->createLocal();		
		entity_manager->assign<DebugComponent>(
			bone.id(),
			Geometry::DebugMesh::Ptr(
				new Geometry::DebugSphere(
					actor_data->getVertices()[bone_it.second->local_pos_index], 10
					)));
	}


	// display all vertices of bone id
	display_vertices_of_bone(4, actor_data, entity_manager);
}

void AITDViewer::run_frame(float dt) {

	float view[16];
	float proj[16];
	bx::mtxProj(proj, 60.0f, float(GraphicsEngine::WIDTH)/float(GraphicsEngine::HEIGHT),
				0.1f, 1000000.0f, bgfx::getCaps()->homogeneousDepth);
	GraphicsEngine::camera().mtxLookAt(view);
	
	bgfx::setViewTransform(RENDER_PASS_GEOMETRY, view, proj);

	Eigen::Matrix4f t = Eigen::Matrix4f::Identity();
	
	// Render all meshes
	entity_manager->each<MeshComponent>(
		[dt, t](Entity entity,
				MeshComponent &mc) {
			mc.render(dt, t.data());
		});

	// Render debug elements
	entity_manager->each<DebugComponent>(
		[dt](Entity entity,
				DebugComponent &dc) {
			dc.render(dt);
		});
}
