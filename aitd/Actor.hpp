#pragma once

#include "utils/Geometry.hpp"
#include <map>

class Bone {
public:
	typedef std::shared_ptr<Bone> Ptr;
	Bone::Ptr parent = nullptr;
	Eigen::Quaternionf local_rot;
	int local_pos_index = 0;
	int start_vertex_index = 0;
	int num_vertices_affected = 0;
	Eigen::Matrix3f local_rotation;
	Eigen::Matrix3f global_rotation;	
};

class Skeleton {
public:
	typedef std::shared_ptr<Skeleton> Ptr;
	std::map<int, Bone::Ptr> bone_map;

};

class Actor {
public:
	typedef std::shared_ptr<Actor> Ptr;
	Actor();
	void generateMesh();
	void updateVertices();
	void draw(float delta);
	Geometry::Mesh::Ptr getMesh() { return mesh; }
	Skeleton::Ptr getSkeleton() { return skeleton; }

	//TODO delete this when done with debugging
	std::vector<Eigen::Vector3f>& getVertices() { return vertices; }
	
protected:

	friend class ActorLoader;
	
	int num_vertices = 0;
	int num_bones = 0;
	int num_primitives = 0;

	//TODO move this to a component-based arch.
	std::vector<Eigen::Vector3f> offsets;
	std::vector<Eigen::Vector3f> vertices;
	std::vector<Geometry::Primitive::Ptr> primitives;
	Skeleton::Ptr skeleton;	
	Geometry::Mesh::Ptr mesh;

	bool refreshVertexBuffer = false;

	//map that maps indices of the original loaded mesh with the vertices with the
	//colored/triangulated mesh. Used to update the dynamic vertex buffer with the
	//animation data
	std::map<int, std::vector<int>> vertex_index_map;
	
};
