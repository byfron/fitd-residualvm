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

void Actor::generateMesh() {
	
	std::vector<PosColorVertex> vertices;
	std::vector<uint16_t> indices;
	int verticesCount = 0;

		
	for (auto prim : primitives) {

		verticesCount = vertices.size();
			
		if (prim->type == PRIM_TYPE_POLYGON) {
			auto poly_ptr = std::dynamic_pointer_cast<Geometry::Polygon>(prim);
				
			uint32_t color = getColor(poly_ptr->color, poly_ptr->poly_type);				
				
			for (auto p : poly_ptr->points) {
				PosColorVertex pcv;
				pcv.m_x = p(0);
				pcv.m_y = p(1);
				pcv.m_z = p(2);										
				pcv.m_abgr = color;
				vertices.push_back(pcv);
			}

			//triangulate
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
	mem = bgfx::copy(&vertices[0], sizeof(PosColorVertex) * vertices.size() );
	mesh->m_dvbh = bgfx::createDynamicVertexBuffer(mem, PosColorVertex::ms_decl);
		
	mem = bgfx::copy(&indices[0], sizeof(uint16_t) * indices.size() );
	mesh->m_dibh = bgfx::createDynamicIndexBuffer(mem);

	mesh->m_shader = Shader::Ptr(new Shader("vs_cubes", "fs_cubes"));
	mesh->m_shader->init();
}

void Actor::draw(float delta) {
	mesh->submit();
}
