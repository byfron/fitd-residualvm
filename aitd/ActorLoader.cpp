#include "ActorLoader.hpp"
#include <common/endian.h>
#include <fitd.h>
#include <map>

Fitd::hqrEntryStruct* ActorLoader::list_body = new Fitd::hqrEntryStruct("LISTBODY", 100000, 50); // was calculated from free mem size

Actor::Ptr ActorLoader::create(int actor_idx) {

	Actor::Ptr actor = Actor::Ptr(new Actor());
	char *buffer = ActorLoader::list_body->get(actor_idx);

	// put here all the creation related code!!!
	
	char *ptr = buffer;
	int idx = 0;
		
	// 2 bytes: flags
	int flags = (int16)READ_LE_UINT16(ptr + idx);

	// Jump 0xE(14)
	idx += 0xE;

	// Jump value returned + 2
	idx += (int16)READ_LE_UINT16(ptr + idx) + 2;

	// Num vertices
	actor->num_vertices = *(int16 *)(ptr + idx);
	idx +=2;

	printf("Num. vertices: %d\n", actor->num_vertices);

	actor->vertices.clear();
		
	// float every 2 bytes
	for (int i = 0; i < actor->num_vertices; i++) {

		float x =  *(int16 *)(ptr + idx + 0);
		float y =  *(int16 *)(ptr + idx + 2);
		float z =  *(int16 *)(ptr + idx + 4);

		actor->vertices.push_back(Eigen::Vector3f(x, -y, z)/1000.0f);
		idx += 6;
	}

	// check bones
	if (flags & 2) {

		actor->num_bones = *(int16 *)(ptr + idx);
		idx += 2;

		printf("Num. bones: %d\n", actor->num_bones);
			
		// next num_bones * uint16_t are the offsets for the bone data
		idx += actor->num_bones * 2;

		std::map<int, Bone> bone_map;
			
		// initialize bone map
		for (int b = 0; b < actor->num_bones; b++) {
			bone_map[b] = Bone();
		}
			
		for (int b = 0; b < actor->num_bones; b++) {

			int start_index = *(int16 *)(ptr + idx + 0) / 6;
			int num_points = *(int16 *)(ptr + idx + 2);
			int vertex_index = *(int16 *)(ptr + idx + 4) / 6;
			int parent_index = *(ptr + idx + 6);
			int bone_index = *(ptr + idx + 7);

			Bone* bone = &bone_map[bone_index];
			bone->parent = &bone_map[parent_index];
			bone->local_rot = Eigen::Quaternionf::Identity();
			bone->local_pos = actor->vertices[vertex_index];

			//apply transform to vertices
			Eigen::Vector3f position = actor->vertices[vertex_index];
			Eigen::Matrix<int, Eigen::Dynamic, 1> blending_weights =
				Eigen::Matrix<int, Eigen::Dynamic, 1>::Zero(num_points, 1);
				
			for (int u = 0; u < num_points; u++) {
				actor->vertices[start_index] += position;
				//blending_weights[start_index] = bone_index;
				start_index++;
			}

			idx += 0x10;				
		}
	}
	else {
		//assert(false);
		printf("No bones\n");

	}
			
	// primitives
		
	actor->num_primitives =  *(int16 *)(ptr + idx);
	idx += 2;

	printf("Num. primitives: %d\n", actor->num_primitives);
		
	for (int n = 0; n < actor->num_primitives; n++) {
			
		int primitive_type = *(ptr + idx);
		idx++;

		//printf("primitive %d\n", primitive_type);
			
		switch(primitive_type) {
				
		case 0: //line
		{
			idx++;
			int color_index = *(ptr + idx);
			idx += 2;
				
			int point_idxA = *(int16 *)(ptr + idx + 0) / 6;
			int point_idxB = *(int16 *)(ptr + idx + 2) / 6;
				
			idx += 4;

			Geometry::Primitive::Ptr primitive_ptr =
				Geometry::Primitive::Ptr(new Geometry::Line(actor->vertices[point_idxA],
															actor->vertices[point_idxB],
															color_index));
			actor->primitives.push_back(primitive_ptr);					
								
			break;
		}
		case 1: //poly
		{
			int num_points_poly = *(ptr + idx);
			uint8 poly_type = *(ptr + idx + 1);
			uint8 color_index = *(ptr + idx + 2);
			idx += 3;

			std::vector<Vector3f> poly_points;

			for (int m = 0; m < num_points_poly; m++) {					
				int point_index = *(int16 *)(ptr + idx) / 6;
				idx += 2;					
				poly_points.push_back(actor->vertices[point_index]);
			}

			Geometry::Primitive::Ptr primitive_ptr =
				Geometry::Primitive::Ptr(new Geometry::Polygon(poly_points,
															   poly_type,
															   color_index));
			actor->primitives.push_back(primitive_ptr);				
				
			break;
		}
		case 2: //point
		{
			break;
		}
		case 3: //sphere
		{
			idx++;
			int color_index = *(ptr + idx);
			idx +=2;
			int size = *(int16 *)(ptr + idx);
			idx +=2;
			int point_index = *(int16 *)(ptr + idx) / 6;
			idx +=2;
				
			Eigen::Vector3f position = actor->vertices[point_index];
				
				
			break;
		}
		case 4:
		case 5:
		case 6:
		case 7:
			break;
		default:
			assert(false); //throw except.
		}				
	}
 	actor->generateMesh();
	return actor;
	
}
