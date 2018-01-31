#include "RenderSystem.hpp"
#include "AITDEngine.hpp"
#include "utils/Color.hpp"
#include <Components.hpp>

using namespace Components;

uint32_t getColor(int color_index, int poly_type) {

	ColorPalette::Ptr palette = ResourceManager::getResource<ColorPalette>(0);		
	Color c = palette->getColor(color_index);
	
	//TODO noise material
	// if (poly_type == 1) {
	// 	c.a = 254;
	// 	c.r = (unsigned char)((color_index % 16) * 16);
	// 	c.g = (unsigned char)((color_index / 16) * 16);
	// }
	// if (poly_type == 4 || poly_type == 5) {
	// 	c.a = 253;
	// 	c.r = 0;
	// 	c.g = 255;
	// 	c.b = (unsigned char)((color_index / 16) * 16);
	// }
	
	return Color::toABGR32(c);	
}


// TODO: Render systems is super simple.
// Long time goal is to add lots of features (look at pumpkin)

void MeshComponent::generateMesh() {

	std::vector<PosColorVertex> vertex_buffer;
	std::vector<uint16_t> indices;
	int verticesCount = 0;
		
	for (auto prim : primitives) {

		verticesCount = vertex_buffer.size();
			
		if (prim->type == PRIM_TYPE_POLYGON) {
			auto poly_ptr = std::dynamic_pointer_cast<Polygon>(prim);
				
			uint32_t color = getColor(poly_ptr->color, poly_ptr->poly_type);				
				
			for (auto p : poly_ptr->points) {
				PosColorVertex pcv;
				pcv.m_x = vertices[p](0);
				pcv.m_y = vertices[p](1);
				pcv.m_z = vertices[p](2);										
				pcv.m_abgr = color;
				vertex_buffer.push_back(pcv);
			}

			//triangulate polygon
			int v0 = 0;
			int v1 = 1;
			int v2 = poly_ptr->points.size() - 1;
			bool swap = true;
			while (v1 < v2)
			{
				indices.push_back(verticesCount + v0);
				indices.push_back(verticesCount + v1);
				indices.push_back(verticesCount + v2);
				if (swap)
				{
					v0 = v1;
					v1++;
				}
				else
				{
					v0 = v2;
					v2--;
				}
				swap = !swap;
			}				
		}
	}

	mesh = std::make_shared<Geometry::Mesh>();
	const bgfx::Memory* mem;
	mem = bgfx::copy(&vertex_buffer[0], sizeof(PosColorVertex) * vertex_buffer.size() );
	mesh->m_dvbh = bgfx::createDynamicVertexBuffer(mem, PosColorVertex::ms_decl);
		
	mem = bgfx::copy(&indices[0], sizeof(uint16_t) * indices.size() );
	mesh->m_dibh = bgfx::createDynamicIndexBuffer(mem);

	mesh->m_shader = Shader::Ptr(new Shader("vs_cubes", "fs_cubes"));
	mesh->m_shader->init();

	updateVertices();
}

void MeshComponent::updateVertices() {

	// // Make sure rotations are up-to-date. TODO: This can be optimized
	for (auto& bone_it : skeleton->bone_map) {
		Bone::Ptr bone = bone_it.second;
		int parent_index = bone->parent_index;
		Eigen::Quaternionf R = bone->local_rotation;
		
		while(parent_index != -1) {
			Bone::Ptr parent = skeleton->bone_map[parent_index];
			R = R * parent->local_rotation;
			parent_index = parent->parent_index;
		}
		bone->global_rotation = R;
	}

	// modify vertex pool
	for (auto& bone_it : skeleton->bone_map) {
		Bone::Ptr bone = bone_it.second;
		int index = bone_it.second->start_vertex_index;
		for (int i = 0; i < bone_it.second->num_vertices_affected; i++) {
			vertices[index] = bone->global_rotation * offsets[index] + vertices[bone->local_pos_index];
			index++;
		}
	}	
	
	std::vector<PosColorVertex> vertex_buffer;
	for (auto prim : primitives) {
		if (prim->type == PRIM_TYPE_POLYGON) {
			auto poly_ptr = std::dynamic_pointer_cast<Polygon>(prim);				
			uint32_t color = getColor(poly_ptr->color, poly_ptr->poly_type);				
			for (auto p : poly_ptr->points) {
				PosColorVertex pcv;
				pcv.m_x = vertices[p](0);
				pcv.m_y = vertices[p](1);
				pcv.m_z = vertices[p](2);										
				pcv.m_abgr = color;
				vertex_buffer.push_back(pcv);
			}
		}
	}
   
	const bgfx::Memory* mem;
	mem = bgfx::copy(&vertex_buffer[0], sizeof(PosColorVertex) * vertex_buffer.size() );
	bgfx::updateDynamicVertexBuffer(mesh->m_dvbh, 0, mem);
}


void RenderSystem::update(EntityManager & em, EventManager &evm, float delta) {

	world->render(delta);

	// Update model mesh with current animation
	em.each<MeshComponent, AnimationComponent>(
		[delta](Entity entity,
				MeshComponent &mc,
				AnimationComponent &ac) {
			
			// perform interpolation
			ac.elapsed_time += delta;
			Skeleton::Ptr skel = ac.getInterpolatedSkeleton();
				
			// update mesh bones
			mc.updateSkeleton(skel);
		});
	
	// Render all meshes
	em.each<MeshComponent, TransformComponent>(
		[delta](Entity entity,
				MeshComponent &mc,
				TransformComponent &tc) {
			
			//get transform			
			mc.render(delta, tc.getTransform().data());
		});


	// Render actor collision components
	em.each<ActorCollisionComponent>(
		[](Entity entity,
				ActorCollisionComponent &ac) {
			Geometry::BBox tb = ac.bounding_box.getTransformedBox();
			DebugManager::push_aabb(tb.p_min,
									tb.p_max);
		});

	em.each<SceneCollisionComponent>(
		[](Entity entity,
				SceneCollisionComponent &sc) {
			Geometry::BBox tb = sc.bounding_box.getTransformedBox();
			DebugManager::push_aabb(tb.p_min,
									tb.p_max);
		});


	// Render actor position
	auto tc_ptr = em.getComponentPtr<TransformComponent>(AITDEngine::player_entity_id);
	DebugManager::push_sphere(Sphere{tc_ptr->getPosition()(0), tc_ptr->getPosition()(1), tc_ptr->getPosition()(2), 40.0});

	// Render Camera Zones
	em.each<CameraZoneComponent>(
		[](Entity entity,
		   CameraZoneComponent &czc) {
			for (auto poly : czc.zones) {
			 	DebugManager::push_polygon(poly.points);
			}			
		});
		
	
	// // Render debug elements
	// em.each<DebugComponent>(
	// 	[delta](Entity entity,
	// 			DebugComponent &dc) {
	// 		dc.render(delta);
	// 	});
}
