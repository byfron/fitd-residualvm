#pragma once

#include <vector>
#include <map>
#include <Eigen/Dense>
#include <common/endian.h>

class Bone {
public:

	Bone* parent;
	Eigen::Quaternionf local_rot;
	Eigen::Vector3f local_pos;
	
};

class Primitive {


};


class AITModel {
public:

	void loadBody(char *buffer) {
		
		char *ptr = buffer;
		int idx = 0;
		
		// 2 bytes: flags
		int flags = (int16)READ_LE_UINT16(ptr + idx);

		// Jump 0xE(14)
		idx += 0xE;

		// Jump value returned + 2
		idx += (int16)READ_LE_UINT16(ptr + idx) + 2;

		// Num vertices
		num_vertices = *(int16 *)(ptr + idx);
		idx +=2;

		vertices.clear();
		
		// float every 2 bytes
		for (int i = 0; i < num_vertices; i++) {

			float x =  *(int16 *)(ptr + idx + 0);
			float y =  *(int16 *)(ptr + idx + 2);
			float z =  *(int16 *)(ptr + idx + 4);

			vertices.push_back(Eigen::Vector3f(x,y,z)/1000.0f);
			idx += 6;
		}

		// check bones
		if (flags & 2) {

			num_bones = *(int16 *)(ptr + idx);
			idx += 2;

			// next num_bones * uint16_t are the offsets for the bone data
			idx += num_bones * 2;

			std::map<int, Bone> bone_map;
			
			// initialize bone map
			for (int b = 0; b < num_bones; b++) {
				bone_map[b] = Bone();
			}
			
			for (int b = 0; b < num_bones; b++) {

				int start_index = *(int16 *)(ptr + idx + 0) / 6;
				int num_points = *(int16 *)(ptr + idx + 2);
				int vertex_index = *(int16 *)(ptr + idx + 4) / 6;
				int parent_index = *(ptr + idx + 6);
				int bone_index = *(ptr + idx + 7);

				Bone* bone = &bone_map[bone_index];
				bone->parent = &bone_map[parent_index];
				bone->local_rot = Eigen::Quaternionf::Identity();
				bone->local_pos = vertices[vertex_index];

				//apply transform to vertices
				Eigen::Vector3f position = vertices[vertex_index];
				Eigen::Matrix<int, Eigen::Dynamic, 1> blending_weights =
					Eigen::Matrix<int, Eigen::Dynamic, 1>::Zero(num_points, 1);
				
				for (int u = 0; u < num_points; u++) {
					vertices[start_index] += position;
					blending_weights[start_index] = bone_index;
					start_index++;
				}

				idx += 0x10;				
			}
		}
		else {
			assert(false);
		}
			
		// primitives
		
		int num_primitives =  *(int16 *)(ptr + idx);
		idx += 2;
		
		for (int n = 0; n < num_primitives; n++) {
			
			int primitive_type = *(ptr + idx);
			idx++;
			
			switch(primitive_type) {
				
			case 0: //line
			{
				idx++;
				int color_index = *(ptr + idx);
				idx += 2;
				
				//Color32 color = getColorFromIndex(color_index);//paletteColors[color_index];
				
				int point_idxA = *(int16 *)(ptr + idx + 0) / 6;
				int point_idxB = *(int16 *)(ptr + idx + 2) / 6;
				
				idx += 4;
				break;
			}
			case 1: //poly
			{
				int num_points_poly = *(ptr + idx);
				int poly_type = *(ptr + idx + 1);
				int color_index = *(ptr + idx + 2);
				idx += 3;

				std::vector<Eigen::Vector3f> polygon;
				
				for (int m = 0; m < num_points_poly; m++) {
					
					int point_index = *(int16 *)(ptr + idx) / 6;
					idx += 2;
					
					polygon.push_back(vertices[point_index]);
				}
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
				
				Eigen::Vector3f position = vertices[point_index];
				
				
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
	}						
	
protected:

	int num_vertices = 0;
	int num_bones = 0;
	int num_primitives = 0;
	
	std::vector<Eigen::Vector3f> vertices;
	std::vector<Primitive> primitives;
	std::vector<Bone> bones;
	
	
};
