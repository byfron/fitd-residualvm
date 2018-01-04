#pragma once

#include "utils/Geometry.hpp"

class Bone {
public:

	Bone* parent;
	Eigen::Quaternionf local_rot;
	Eigen::Vector3f local_pos;
	
};

class Actor {
public:
	typedef std::shared_ptr<Actor> Ptr;
	void generateMesh();
	void draw(float delta);
	Geometry::Mesh::Ptr getMesh() { return mesh; }
	
protected:

	friend class ActorLoader;
	
	int num_vertices = 0;
	int num_bones = 0;
	int num_primitives = 0;

	//TODO move this to a component-based arch.   
	std::vector<Eigen::Vector3f> vertices;
	std::vector<Geometry::Primitive::Ptr> primitives;
	std::vector<Bone> bones;
	Geometry::Mesh::Ptr mesh;
	
};
