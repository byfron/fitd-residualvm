#include "ActorLoader.hpp"
#include <common/endian.h>
#include <utils/DataParsing.hpp>
#include <fitd.h>
#include <map>

//this could be private
Fitd::hqrEntryStruct* ActorLoader::list_body = new Fitd::hqrEntryStruct("LISTBODY", 100000, 50); // was calculated from free mem size
Fitd::hqrEntryStruct* ActorLoader::list_anim = new Fitd::hqrEntryStruct("LISTANIM", 100000, 50); // was calculated from free mem size


enum RotationType {
	ROTATION_BONE_TYPE = 0,
	TRANSLATION_BONE_TYPE1,
	TRANSLATION_BONE_TYPE2
};

Eigen::Matrix3f getRotationMatrixFromRotIndices(uint16 rotx, uint16 roty, uint16 rotz) {

	float cosx = DataParsing::computeCos(rotx&0x3FF);
	float cosy = DataParsing::computeCos(roty&0x3FF);
	float cosz = DataParsing::computeCos(rotz&0x3FF);						
	float angle_x = asinf(cosx);
	float angle_y = asinf(cosy);
	float angle_z = asinf(cosz);					
	if (rotx >= 0x3FF) angle_x = 0;
	if (roty >= 0x3FF) angle_y = 0;
	if (rotz >= 0x3FF) angle_z = 0;

	//NOTE: I had to switch here the X=-Z axis. Cameras dont seem to follow same rotation pattern.	
	Eigen::Matrix3f rotX = Eigen::Matrix3f(AngleAxisf(angle_x, Vector3f::UnitZ()));
	Eigen::Matrix3f rotY = Eigen::Matrix3f(AngleAxisf(angle_y, Vector3f::UnitY()));
	Eigen::Matrix3f rotZ = Eigen::Matrix3f(AngleAxisf(angle_z, -Vector3f::UnitX()));

	if (!roty) {
		rotY.col(0) = Vector3f::UnitX();
		rotY.col(2) = Vector3f::UnitZ();
		rotY = Eigen::Matrix3f::Identity();
	}
	if (!rotx) {
		rotX.col(1) = Vector3f::UnitY();
		rotX.col(2) = Vector3f::UnitZ();
		rotX = Eigen::Matrix3f::Identity();
	}
	if (!rotz) {
		rotZ.col(0) = Vector3f::UnitX();
		rotZ.col(1) = Vector3f::UnitY();
		rotZ = Eigen::Matrix3f::Identity();
	}
			
	return Eigen::Matrix3f(rotZ * rotY * rotX);
}


Animation::Ptr ActorLoader::loadAnimation(Skeleton::Ptr skeleton, int anim_idx) {

	Animation::Ptr anim = Animation::Ptr(new Animation());
	
	char* start_of_buffer = ActorLoader::list_anim->get(anim_idx);
	char* anim_buffer  = start_of_buffer;
	int16 num_frames_in_anim = *(int16 *)(anim_buffer);

	for (int i = 0; i < num_frames_in_anim; i++)
	{

		char* anim_buffer  = start_of_buffer;
		// TODO: re-factor this so that we can fetch specific places of a buffer (using a table maybe)
		int16 num_bones_in_anim = *(int16 *)(anim_buffer + 2);
		anim_buffer += 4;
		anim_buffer += ((num_bones_in_anim + 1) * 8) * i; // seek keyframe

		// keyframe length (time keyframe should last until we change)
		uint16 keyframeLength = *(uint16 *)anim_buffer;
		
		anim_buffer += 4;
	
		KeyFrame::Ptr kframe = KeyFrame::Ptr(new KeyFrame());
		kframe->length = keyframeLength;
		kframe->skeleton = skeleton->deepCopy();
		
		for (auto bone_it : kframe->skeleton->bone_map) {

			uint16 type = *(uint16 *)anim_buffer;
			anim_buffer += 2;
		
			//get new rotation values
			uint16 new_rotx = *(uint16 *)anim_buffer;
			anim_buffer += 2;
		
			uint16 new_roty = *(uint16 *)anim_buffer;
			anim_buffer += 2;

			uint16 new_rotz = *(uint16 *)anim_buffer;
			anim_buffer += 2;

			bone_it.second->local_rotation = getRotationMatrixFromRotIndices(new_rotx, new_roty, new_rotz);
		}

		anim->keyframes.push_back(kframe);
	}

	return anim;	
}

Actor::Ptr ActorLoader::load(int actor_idx) {
   	
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

	actor->offsets.clear();

	// Locations of mesh vertices as offsets from its corresponding bone head 
	// float every 2 bytes
	for (int i = 0; i < actor->num_vertices; i++) {

		float x =  *(int16 *)(ptr + idx + 0);
		float y =  *(int16 *)(ptr + idx + 2);
		float z =  *(int16 *)(ptr + idx + 4);

		actor->offsets.push_back(Eigen::Vector3f(x, -y, z));
		actor->vertices.push_back(Eigen::Vector3f(x, -y, z));
		
		idx += 6;
	}

	// check bones
	if (flags & 2) {

		actor->num_bones = *(int16 *)(ptr + idx);
		idx += 2;

		int16 bonesOffsets[100];
		memcpy(bonesOffsets, (int16 *)(ptr + idx), actor->num_bones * 2);
		// next num_bones * uint16_t are the offsets for the bone data
		idx += actor->num_bones * 2;
		
		std::map<int, Bone::Ptr> &bone_map = actor->getSkeleton()->bone_map;
		bone_map.clear();
			
		// initialize bone map
		for (int b = 0; b < actor->num_bones; b++) {
			bone_map[b] = std::shared_ptr<Bone>(new Bone());
		}

		char *ptr_base_bone_data = ptr + idx;
			
		for (int b = 0; b < actor->num_bones; b++) {

			int type = *(int16 *)(ptr_base_bone_data + bonesOffsets[b] + 0x8);
			int16 rotx, roty, rotz;
			
			switch(type) {
			case ROTATION_BONE_TYPE: {
				rotx = *(int16 *)(ptr_base_bone_data + bonesOffsets[b] + 0x10);
				roty = *(int16 *)(ptr_base_bone_data + bonesOffsets[b] + 0x12);
				rotz = *(int16 *)(ptr_base_bone_data + bonesOffsets[b] + 0x14);
				break;
			}
					
			case TRANSLATION_BONE_TYPE1: {
				int transx = *(int16 *)(ptr_base_bone_data + bonesOffsets[b] + 0x10);
				int transy = *(int16 *)(ptr_base_bone_data + bonesOffsets[b] + 0x12);
				int transz = *(int16 *)(ptr_base_bone_data + bonesOffsets[b] + 0x14);
				break;
			}
				
			case TRANSLATION_BONE_TYPE2: {
				int transx = *(int16 *)(ptr_base_bone_data + bonesOffsets[b] + 0x10);
				int transy = *(int16 *)(ptr_base_bone_data + bonesOffsets[b] + 0x12);
				int transz = *(int16 *)(ptr_base_bone_data + bonesOffsets[b] + 0x14);
				break;
			}
			}
						
			Eigen::Matrix3f R = getRotationMatrixFromRotIndices(rotx, roty, rotz);
			
			// starting index of the vertices affected by this bone
		 	int start_index = *(int16 *)(ptr + idx + 0) / 6;

			// number of vertices affected by this bone
 			int num_points = *(int16 *)(ptr + idx + 2);

			// vertex indicating the location of the bone head
 			int vertex_index = *(int16 *)(ptr + idx + 4) / 6;

			// index of the parent bone
 			int parent_index = *(ptr + idx + 6);

			// index of the current bone
 			int bone_index = *(ptr + idx + 7);

			assert(bone_index < actor->num_bones);
			assert(parent_index < actor->num_bones);				

			/* NOTE: The bones are ordered so that parents always come first. The bone heads
			   themselfs are vertices as well. So when rotating a bone, the children bone heads
			   will move acoording to their parent.
			*/			
			Bone::Ptr bone = bone_map[bone_index];

			// Root bone has parent -1
			bone->parent_index = parent_index;
			bone->local_rot = Eigen::Quaternionf::Identity();
			bone->local_pos_index = vertex_index;
			bone->start_vertex_index = start_index;
			bone->num_vertices_affected = num_points;
			bone->local_rotation = Eigen::Matrix3f::Identity();
			bone->global_rotation = Eigen::Matrix3f::Identity();			

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

	int vertex_count = 0;
	
	for (int n = 0; n < actor->num_primitives; n++) {
			
		int primitive_type = *(ptr + idx);
		idx++;

		switch(primitive_type) {
				
		case 0: //line
		{
			idx++;
			int color_index = *(ptr + idx);
			idx += 2;
				
			int point_idxA = *(int16 *)(ptr + idx + 0) / 6;
			int point_idxB = *(int16 *)(ptr + idx + 2) / 6;
				
			idx += 4;

			Primitive::Ptr primitive_ptr =
				Primitive::Ptr(new Line(point_idxA,
										point_idxB,
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

			std::vector<int16> poly_points;
			for (int m = 0; m < num_points_poly; m++) {					
				int point_index = *(int16 *)(ptr + idx) / 6;
				idx += 2;					
				poly_points.push_back(point_index);
			}

			Primitive::Ptr primitive_ptr =
				Primitive::Ptr(new Polygon(poly_points,
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

	return actor;	
}
