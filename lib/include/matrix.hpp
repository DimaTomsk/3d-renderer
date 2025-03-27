#pragma once

#include <array>

namespace Renderer {

template <typename T>
struct Vec3DT {
  T x{};
  T y{};
  T z{};

  Vec3DT operator+(const Vec3DT& other) const {
    return {x + other.x, y + other.y, z + other.z};
  }

  Vec3DT operator-() const { return {-x, -y, -z}; }
};

using Vec3D = Vec3DT<double>;
using Vec3Di = Vec3DT<int32_t>;

struct Vec4D {
  static constexpr double kMinT = 1e-6;

  double x{};
  double y{};
  double z{};
  double t{};

  double& operator[](size_t pos);
  double operator[](size_t pos) const;
};

Vec4D ToVec4D(const Vec3D& vec_3d);
Vec3D ToVec3D(const Vec4D& vec_4d);

struct Matrix {
  static constexpr uint32_t kSize = 4;
  std::array<std::array<double, kSize>, kSize> array;

  static Matrix OxRotation(double angle);
  static Matrix OyRotation(double angle);
  static Matrix MoveMatrix(const Vec3D& vec_3d);
  static Matrix ProjectionMatrix(double near, double far);

  Matrix operator*(const Matrix& other) const;
  Vec4D operator*(const Vec4D& other) const;
  Vec3D operator*(const Vec3D& other) const;
};

}  // namespace Renderer