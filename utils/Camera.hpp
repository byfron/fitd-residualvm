#ifndef CAMERA_H_HEADER_GUARD
#define CAMERA_H_HEADER_GUARD

#include <stdint.h>
#include <Eigen/Geometry>
#include <memory>
#include "math.hpp"

#define CAMERA_KEY_ROTATE_LEFT   UINT8_C(0x01)
#define CAMERA_KEY_ROTATE_RIGHT  UINT8_C(0x02)
#define CAMERA_KEY_LEFT      UINT8_C(0x04)
#define CAMERA_KEY_RIGHT     UINT8_C(0x08)
#define CAMERA_KEY_UP        UINT8_C(0x10)
#define CAMERA_KEY_DOWN      UINT8_C(0x20)

class Camera {

public:
	Camera();
	~Camera();
	void init();
	void reset();
	void mtxLookAt(float* _outViewMtx);
	void orbit(float _dx, float _dy);
	void dolly(float _dz);
	void consumeOrbit(float _amount);
	static void setKeyState(uint8_t _key, bool _down);
	void update(float _dt);

	float getPitch();

	static inline void vecFromLatLong(float _vec[3], float _u, float _v);
	static inline void latLongFromVec(float& _u, float& _v, const float _vec[3]);

	Eigen::MatrixXf getTowardsCameraRotation(Vec3f);

	struct Interp3f
	{
		float curr[3];
		float dest[3];
	};

	void setSpeed(float s) { m_moveSpeed = s; }

	void moveAlong(Vec3f vec) {
		m_eye += vec;
		m_at = m_eye + m_cam_direction*5;
	}

	Eigen::Vector3f & getDirection() { return m_cam_direction; }
	Eigen::Vector3f & getEye() { return m_eye; }
	Eigen::Vector3f & getAt() { return m_at; }
	Eigen::Vector3f & getForwardDir() { return m_cam_forward_dir; }
	Eigen::Vector3f & getRightDir() { return m_cam_right_dir; }

private:

	Interp3f m_target;
	Interp3f m_pos;
	float m_orbit[2];

	float m_horizontalAngle;
	float m_verticalAngle;

	float m_gamepadSpeed;
	float m_moveSpeed;
	float m_rotateSpeed;

	float m_dir_angle;

	Eigen::Vector3f m_eye;
	Eigen::Vector3f m_at;

	Eigen::Vector3f m_cam_up;
	Eigen::Vector3f m_cam_right;
	Eigen::Vector3f m_cam_direction;

	Eigen::Vector3f m_cam_forward_dir;
	Eigen::Vector3f m_cam_right_dir;
};

//Camera* getCamera();
//void createCamera();
//void destroyCamera();


#endif
