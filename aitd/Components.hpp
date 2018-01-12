#pragma once

#include "Actor.hpp" // remove this 

namespace Components {

class UserInputComponent {
public:
};

class AnimationComponent {
public:   	
	AnimationComponent(Animation::Ptr a) : anim(a) {
		//TODO: we should instead deep copy the data here otherwise we break our cache-friendly design

		current_skeleton = Skeleton::Ptr(new Skeleton());
	}

	Skeleton::Ptr getInterpolatedSkeleton() {

		//length is in frames (we should divide by the framerate?)
		alpha = elapsed_time/(float(anim->keyframes[current_frame]->length)/30);
		if (alpha > 1.0f) {
			//next frame
			elapsed_time = 0.0f;


			next_frame += frame_inc;
			if (next_frame == anim->keyframes.size() || next_frame == -1) {
				frame_inc *= -1;
				current_frame = next_frame + frame_inc;
				next_frame = current_frame + frame_inc;
			}
			else {
				current_frame += frame_inc;
			}
			
			
		}

		std::cout << "num kfr:" << current_frame << "," << next_frame << "/" << anim->keyframes.size() << std::endl;
		
		return interpolateSkeletons(anim->keyframes[current_frame]->skeleton,
									anim->keyframes[next_frame]->skeleton,
									alpha);

	
		
	}

	// int anim_type;
	float alpha = 0; //interpolation factor
	int duration = 0;
	float elapsed_time = 0;
	
	int frame_inc = 1;	
	int current_frame = 0;
	int next_frame = 1;
	
	// std::vector<Frame> frames;

	Skeleton::Ptr current_skeleton;
	Animation::Ptr anim;

};
	
class MoveComponent {
public:
	MoveComponent(float ms, float rs) : move_speed(ms),
										rotate_speed(rs),
										move_vector(-Vec3f::UnitZ()) {}
	float move_speed = 0;
	float rotate_speed = 0;
	Vec3f move_vector; //goto_vector

	void reset() {
		translation = Eigen::Vector3f::Zero();
		rotation = Eigen::Matrix3f::Identity();
	}
	
	Eigen::Vector3f translation = Eigen::Vector3f::Zero();
	Eigen::Matrix3f rotation = Eigen::Matrix3f::Identity();
};


class TransformComponent {
public:
	TransformComponent(int16 x, int16 y, int16 z, const Eigen::Matrix3f& rot) :
		translation(Eigen::Vector3f(x, y, z)),
		rotation(rot) {
	}

	Eigen::Matrix4f getTransform() {
		Eigen::Matrix4f T = Eigen::Matrix4f::Identity();
		T.col(3).head(3) = translation;
		T.topLeftCorner(3,3) = rotation;
		return T;
	}

	Eigen::Matrix3f rotation = Eigen::Matrix3f::Identity();
	Eigen::Vector3f translation = Eigen::Vector3f::Zero();
};
	
};
