#pragma once

namespace Components {


class TransformComponent {
public:
	TransformComponent(int16 x, int16 y, int16 z, const Eigen::Matrix3f& rot) :
		translation(Eigen::Vector3f(x, y, z)),
		rotation(rot) {
	}

	void update(float dt) {
		
	}

	Eigen::Matrix4f getTransform() {
		Eigen::Matrix4f T = Eigen::Matrix4f::Identity();
		T.col(3).head(3) = translation;
		T.topLeftCorner(3,3) = rotation;
		return T;
	}

protected:

	Eigen::Matrix3f rotation = Eigen::Matrix3f::Identity();
	Eigen::Vector3f translation = Eigen::Vector3f::Zero();
};
	
};
