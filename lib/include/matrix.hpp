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

  double& operator[](size_t pos) {
    if (pos == 0) {
      return x;
    }
    if (pos == 1) {
      return y;
    }
    if (pos == 2) {
      return z;
    }
    if (pos == 3) {
      return t;
    }
    throw std::runtime_error("Unexpected id");
  }

  const double& operator[](size_t pos) const {
    if (pos == 0) {
      return x;
    }
    if (pos == 1) {
      return y;
    }
    if (pos == 2) {
      return z;
    }
    if (pos == 3) {
      return t;
    }
    throw std::runtime_error("Unexpected id");
  }
};

Vec4D ToVec4D(const Vec3D& vec_3d);
Vec3D ToVec3D(const Vec4D& vec_4d);

struct Matrix {
  static constexpr uint32_t kSize = 4;
  std::array<std::array<double, kSize>, kSize> array;

  static Matrix OxRotation(double angle) {
    return {{{{1, 0, 0, 0},
              {0, std::cos(angle), -std::sin(angle), 0},
              {0, std::sin(angle), std::cos(angle), 0},
              {0, 0, 0, 1}}}};
  }

  static Matrix OyRotation(double angle) {
    return {{{{std::cos(angle), 0, std::sin(angle), 0},
              {0, 1, 0, 0},
              {-std::sin(angle), 0, std::cos(angle), 0},
              {0, 0, 0, 1}}}};
  }

  static Matrix MoveMatrix(const Vec3D& vec_3d) {
    return {{{{1, 0, 0, vec_3d.x},
              {0, 1, 0, vec_3d.y},
              {0, 0, 1, vec_3d.z},
              {0, 0, 0, 1}}}};
  }

  static Matrix ProjectionMatrix(double near, double far) {
    return {{{{far, 0, 0, 0},
              {0, far, 0, 0},
              {0, 0, near + far, -near * far},
              {0, 0, 1, 0}}}};
  }

  static Matrix E() {
    return {{{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}}};
  }

  Matrix operator*(const Matrix& other) const {
    Matrix result{};
    for (uint32_t i = 0; i < kSize; ++i) {
      for (uint32_t j = 0; j < kSize; ++j) {
        for (uint32_t k = 0; k < kSize; ++k) {
          result.array[i][k] += array[i][j] * other.array[j][k];
        }
      }
    }
    return result;
  }

  Vec4D operator*(const Vec4D& other) const {
    Vec4D result{};
    for (uint32_t i = 0; i < 4; ++i) {
      for (uint32_t j = 0; j < 4; ++j) {
        result[i] += array[i][j] * other[j];
      }
    }
    return result;
  }

  Vec3D operator*(const Vec3D& other) const {
    return ToVec3D(*this * ToVec4D(other));
  }
};

}  // namespace Renderer