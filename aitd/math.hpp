#include "Eigen/Dense"


typedef Eigen::Matrix<float, 2, 1>  Vec2f;
typedef Eigen::Vector3f Vec3f;
typedef Eigen::Matrix<float, 4, 1>  Vec4f;

typedef Eigen::Vector3i Vec3i;
typedef Eigen::Matrix<int, 2, 1> Vec2i;

#define VEC_TO_HOMOG(x) Eigen::Matrix<float, 4, 1>(x(0), x(1), x(2), 1.0f)
#define VEC_FROM_HOMOG(x) Eigen::Matrix<float, 3, 1>(x(0), x(1), x(2))

#define TO_2DVEC(x) Vec2f(x(0), x(1))
#define TO_3DVEC(x) Vec3f(x(0), x(1), 0.0)

#define FROM_V3(cv, ev) cv[0] = ev(0); cv[1] = ev(1); cv[2] = ev(2);
