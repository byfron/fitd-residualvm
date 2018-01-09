#pragma once

#include "utils/Geometry.hpp"

class Bone {
public:
	typedef std::shared_ptr<Bone> Ptr;
	Bone::Ptr parent;
	Eigen::Quaternionf local_rot;
	Eigen::Vector3f local_pos;
	
};

class Skeleton {
public:
	typedef std::shared_ptr<Skeleton> Ptr;
	std::vector<Bone::Ptr> bone_list;
};

class Actor {
public:
	typedef std::shared_ptr<Actor> Ptr;
	void generateMesh();
	void draw(float delta);
	Geometry::Mesh::Ptr getMesh() { return mesh; }
	Skeleton::Ptr getSkeleton() { return skeleton; }
	
protected:

	friend class ActorLoader;
	
	int num_vertices = 0;
	int num_bones = 0;
	int num_primitives = 0;

	//TODO move this to a component-based arch.   
	std::vector<Eigen::Vector3f> vertices;
	std::vector<Geometry::Primitive::Ptr> primitives;
	Skeleton::Ptr skeleton;	
	Geometry::Mesh::Ptr mesh;
	
};
