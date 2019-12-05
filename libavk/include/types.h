#ifndef AVK_TYPES_H_
#define AVK_TYPES_H_

#include <vector>

#include <Eigen/Core>

namespace avk {

using Vec2 = Eigen::Vector2f;
using Vec3 = Eigen::Vector3f;
using Vec4 = Eigen::Vector4f;

struct Transform {
    Vec3 location = { 0.0f, 0.0f, 0.0f };
    Vec3 rotation = { 0.0f, 0.0f, 0.0f };
    Vec3 scale = { 1.0f, 1.0f, 1.0f };
};

}
#endif // #ifndef AVK_TYPES_H_
