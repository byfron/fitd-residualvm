#pragma once

#include "utils/Geometry.hpp"
#include <map>

class Primitive {
public:
	virtual ~Primitive() = default;
	
	typedef std::shared_ptr<Primitive> Ptr;
	Primitive(int c, int t) : color(c), type(t) {}
	int type = 0;
	int color = 0;
};


// The primivitves store pointers to the pool of vertices
// so that it's easier to deal with animations
class Line : public Primitive {
public:
	Line(int16_t a, int16_t b, int c) :
		Primitive(c, PRIM_TYPE_LINE),
		point_a(a),
		point_b(b) {}
	int16_t point_a;
	int16_t point_b;
};

class Polygon : public Primitive {
public:
	Polygon(const std::vector<int16_t>& p, int t, int c) :
		Primitive(c, PRIM_TYPE_POLYGON),
		poly_type(t),
		points(p) {}
	std::vector<int16_t> points;
	int poly_type = 0;
};

class Bone {
public:
	typedef std::shared_ptr<Bone> Ptr;
	int parent_index = -1;
	int local_pos_index = 0;
	int start_vertex_index = 0;
	int num_vertices_affected = 0;
	Eigen::Quaternionf local_rotation;
	Eigen::Quaternionf global_rotation;	
};

class Skeleton {
public:
	typedef std::shared_ptr<Skeleton> Ptr;
	Skeleton() {}
	Skeleton(const Skeleton&) = delete; //delete copy constructor to avoid wrong bone pointers
	Skeleton::Ptr deepCopy();
	std::map<int, Bone::Ptr> bone_map;

};


Skeleton::Ptr interpolateSkeletons(Skeleton::Ptr a,
								   Skeleton::Ptr b,
								   float alpha);	


class KeyFrame {
public:
	typedef std::shared_ptr<KeyFrame> Ptr;
	KeyFrame() {}
	uint16_t length;
	Skeleton::Ptr skeleton;
};

class Animation {
public:
	typedef std::shared_ptr<Animation> Ptr;
	Animation() {}
	std::vector<KeyFrame::Ptr> keyframes;
};

// This class (should be a struct probably) simply acts as a temporary container
// of data loaded from AITD files. TODO: take animations out?
class Actor {
public:
	typedef std::shared_ptr<Actor> Ptr;
	Actor();
	void generateMesh();
	void updateVertices();
	void draw(float delta);
	//Geometry::Mesh::Ptr getMesh() { return mesh; }
	Skeleton::Ptr getSkeleton() { return skeleton; }
	void setSkeleton(Skeleton::Ptr s) { skeleton = s; }

	//TODO delete this when done with debugging
	std::vector<Eigen::Vector3f>& getVertices() { return vertices; }

	// void addKeyFrame(uint16_t anim_idx, KeyFrame::Ptr keyframe) {
	// 	if (animations.count(anim_idx) == 0) {
	// 		animations[anim_idx] = Animation::Ptr(new Animation());						
	// 	}
	// 	animations[anim_idx]->keyframes.push_back(keyframe);
	// }

	// Animation::Ptr getAnimation(int idx) {
	// 	assert(animations.count(idx));
	// 	return animations[idx];
	// }
	
	int num_vertices = 0;
	int num_bones = 0;
	int num_primitives = 0;

	//TODO move this to a component-based arch.
	std::vector<Eigen::Vector3f> offsets;
	std::vector<Eigen::Vector3f> vertices;
	std::vector<Primitive::Ptr> primitives;
	Skeleton::Ptr skeleton;	
//	Geometry::Mesh::Ptr mesh;

	//std::map<uint16_t, Animation::Ptr> animations;

	Geometry::BBox bounding_box;
	bool refreshVertexBuffer = false;
	
};
