#include "Actor.hpp"
#include "utils/Color.hpp"

//copies skeleton structure to avoid 
Skeleton::Ptr Skeleton::deepCopy() {
	Skeleton::Ptr copy = Skeleton::Ptr(new Skeleton());
	for (auto bone : bone_map) {
		copy->bone_map[bone.first] = Bone::Ptr(new Bone(*bone.second));
	}
	return copy;
}


Skeleton::Ptr interpolateSkeletons(Skeleton::Ptr a,
								   Skeleton::Ptr b,
								   float alpha) {

	//assert (a.nbones == b.nbones)	
	Skeleton::Ptr interp = a->deepCopy();	
	for (auto bone_it : a->bone_map) {
		Eigen::Quaternionf q_a = Eigen::Quaternionf(bone_it.second->local_rotation);
		Eigen::Quaternionf q_b = Eigen::Quaternionf(b->bone_map[bone_it.first]->local_rotation);
		Eigen::Quaternionf q_interp = q_a.slerp(alpha,q_b);
		interp->bone_map[bone_it.first]->local_rotation = Eigen::Matrix3f(q_interp);		
	}

	return interp;   
}

Actor::Actor() {
	skeleton = std::make_shared<Skeleton>();
}

void Actor::generateMesh() {
	
	// std::vector<PosColorVertex> vertex_buffer;
	// std::vector<uint16_t> indices;
	// int verticesCount = 0;
		
	// for (auto prim : primitives) {

	// 	verticesCount = vertex_buffer.size();
			
	// 	if (prim->type == PRIM_TYPE_POLYGON) {
	// 		auto poly_ptr = std::dynamic_pointer_cast<Geometry::Polygon>(prim);
				
	// 		uint32_t color = getColor(poly_ptr->color, poly_ptr->poly_type);				
				
	// 		for (auto p : poly_ptr->points) {
	// 			PosColorVertex pcv;
	// 			pcv.m_x = (*p)(0);
	// 			pcv.m_y = (*p)(1);
	// 			pcv.m_z = (*p)(2);										
	// 			pcv.m_abgr = color;
	// 			vertex_buffer.push_back(pcv);
	// 		}

	// 		//triangulate polygon
	// 		int v0 = 0;
	// 		int v1 = 1;
	// 		int v2 = poly_ptr->points.size() - 1;
	// 		bool swap = true;
	// 		while (v1 < v2)
	// 		{
	// 			indices.push_back(verticesCount + v0);
	// 			indices.push_back(verticesCount + v1);
	// 			indices.push_back(verticesCount + v2);
	// 			if (swap)
	// 			{
	// 				v0 = v1;
	// 				v1++;
	// 			}
	// 			else
	// 			{
	// 				v0 = v2;
	// 				v2--;
	// 			}
	// 			swap = !swap;
	// 		}				
	// 	}
	// }

	// mesh = std::make_shared<Geometry::Mesh>();
	// const bgfx::Memory* mem;
	// mem = bgfx::copy(&vertex_buffer[0], sizeof(PosColorVertex) * vertex_buffer.size() );
	// mesh->m_dvbh = bgfx::createDynamicVertexBuffer(mem, PosColorVertex::ms_decl);
		
	// mem = bgfx::copy(&indices[0], sizeof(uint16_t) * indices.size() );
	// mesh->m_dibh = bgfx::createDynamicIndexBuffer(mem);

	// mesh->m_shader = Shader::Ptr(new Shader("vs_cubes", "fs_cubes"));
	// mesh->m_shader->init();
}

void Actor::updateVertices() {

	// // Make sure rotations are up-to-date. TODO: This can be optimized
	// for (auto& bone_it : skeleton->bone_map) {
	// 	Bone::Ptr bone = bone_it.second;
	// 	int parent_index = bone->parent_index;
	// 	Eigen::Matrix3f R = bone->local_rotation;
		
	// 	while(parent_index != -1) {
	// 		Bone::Ptr parent = skeleton->bone_map[parent_index];
	// 		std::cout << "parent idx: " << parent_index << std::endl;
	// 		R = R * parent->local_rotation;
	// 		parent_index = parent->parent_index;
	// 	}
	// 	bone->global_rotation = R;
	// }

	// // modify vertex pool
	// for (auto& bone_it : skeleton->bone_map) {
	// 	Bone::Ptr bone = bone_it.second;
	// 	int index = bone_it.second->start_vertex_index;
	// 	for (int i = 0; i < bone_it.second->num_vertices_affected; i++) {
	// 		vertices[index] = bone->global_rotation * offsets[index] + vertices[bone->local_pos_index];
	// 		index++;
	// 	}
	// }	
	
	// std::vector<PosColorVertex> vertex_buffer;
	// for (auto prim : primitives) {
	// 	if (prim->type == PRIM_TYPE_POLYGON) {
	// 		auto poly_ptr = std::dynamic_pointer_cast<Polygon>(prim);				
	// 		uint32_t color = getColor(poly_ptr->color, poly_ptr->poly_type);				
	// 		for (auto p : poly_ptr->points) {
	// 			PosColorVertex pcv;
	// 			pcv.m_x = (*p)(0);
	// 			pcv.m_y = (*p)(1);
	// 			pcv.m_z = (*p)(2);										
	// 			pcv.m_abgr = color;
	// 			vertex_buffer.push_back(pcv);
	// 		}
	// 	}
	// }
   
	// const bgfx::Memory* mem;
	// mem = bgfx::copy(&vertex_buffer[0], sizeof(PosColorVertex) * vertex_buffer.size() );
	// bgfx::updateDynamicVertexBuffer(mesh->m_dvbh, 0, mem);
	
	// refreshVertexBuffer = true;
}

void Actor::draw(float delta) {

	//update the vertex buffer if the actor has been changed
	// if (refreshVertexBuffer) {
	// 	const bgfx::Memory* mem;
	// 	mem = bgfx::copy(&vertices[0], sizeof(PosColorVertex) * vertices.size() );
	// 	bgfx::updateDynamicVertexBuffer(mesh->m_dvbh, 0, mem);
	// 	refreshVertexBuffer = false;
	// }
	
//	mesh->submit();
}
