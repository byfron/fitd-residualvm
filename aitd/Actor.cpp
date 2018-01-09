#include "Actor.hpp"
#include "utils/Color.hpp"

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

Actor::Actor() {
	skeleton = std::make_shared<Skeleton>();
}

void Actor::generateMesh() {
	
	std::vector<PosColorVertex> vertex_buffer;
	std::vector<uint16_t> indices;
	int verticesCount = 0;
		
	for (auto prim : primitives) {

		verticesCount = vertex_buffer.size();
			
		if (prim->type == PRIM_TYPE_POLYGON) {
			auto poly_ptr = std::dynamic_pointer_cast<Geometry::Polygon>(prim);
				
			uint32_t color = getColor(poly_ptr->color, poly_ptr->poly_type);				
				
			for (auto p : poly_ptr->points) {
				PosColorVertex pcv;
				pcv.m_x = p(0);
				pcv.m_y = p(1);
				pcv.m_z = p(2);										
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
}

void Actor::updateVertices() {

	// Make sure rotations are up-to-date. TODO: This can be optimized
	for (auto& bone_it : skeleton->bone_map) {
		Bone::Ptr bone = bone_it.second;
		Bone::Ptr parent = bone->parent;
		Eigen::Matrix3f R = bone->local_rotation;	   
		while(parent) {
			R = R * parent->local_rotation;
			parent = parent->parent;
		}
		bone->global_rotation = R;
	}

	for (auto prim : primitives) {
		verticesCount = vertex_buffer.size();			
		if (prim->type == PRIM_TYPE_POLYGON) {
			auto poly_ptr = std::dynamic_pointer_cast<Geometry::Polygon>(prim);				
			uint32_t color = getColor(poly_ptr->color, poly_ptr->poly_type);				
			for (auto p : poly_ptr->points) {
				PosColorVertex pcv;
				pcv.m_x = p(0);
				pcv.m_y = p(1);
				pcv.m_z = p(2);										
				pcv.m_abgr = color;
				vertex_buffer.push_back(pcv);
			}
		}
	}

	// std::vector<PosColorVertex> vertex_buffer;
	// for (auto& bone_it : skeleton->bone_map) {
	// 	Bone::Ptr bone = bone_it.second;
	// 	int index = bone_it.second->start_vertex_index;
	// 	for (int i = 0; i < bone_it.second->num_vertices_affected; i++) {
	// 		std::vector<int> indices_in_vbuffer = vvertex_index_map[index];
	// 		for (idx : indices_in_vbuffer) {

	// 			idx
					
	// 			vertex_buffer.push_back()
					
	// 			vertices[index] = bone->global_rotation * offsets[index] + vertices[bone->local_pos_index];
	// 		}
	// 		index++;
	// 	}
	// }
	
	const bgfx::Memory* mem;
	mem = bgfx::copy(&vertex_buffer[0], sizeof(PosColorVertex) * vertex_buffer.size() );
	bgfx::updateDynamicVertexBuffer(mesh->m_dvbh, 0, mem);
	
	refreshVertexBuffer = true;
}

void Actor::draw(float delta) {

	//update the vertex buffer if the actor has been changed
	// if (refreshVertexBuffer) {
	// 	const bgfx::Memory* mem;
	// 	mem = bgfx::copy(&vertices[0], sizeof(PosColorVertex) * vertices.size() );
	// 	bgfx::updateDynamicVertexBuffer(mesh->m_dvbh, 0, mem);
	// 	refreshVertexBuffer = false;
	// }
	
	mesh->submit();
}
