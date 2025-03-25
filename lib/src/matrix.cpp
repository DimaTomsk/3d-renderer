#include "matrix.hpp"

#include <cassert>

namespace Renderer {

Vec4D ToVec4D(const Vec3D& vec_3d) {
  return {vec_3d.x, vec_3d.y, vec_3d.z, 1};
}
Vec3D ToVec3D(const Vec4D& vec_4d) {
  assert(std::abs(vec_4d.t) > Vec4D::kMinT);
  return {vec_4d.x / vec_4d.t, vec_4d.y / vec_4d.t, vec_4d.z / vec_4d.t};
}

};  // namespace Renderer