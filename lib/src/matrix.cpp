#include "matrix.hpp"

#include <cassert>

namespace Renderer {

double& Vec4D::operator[](size_t pos) {
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

double Vec4D::operator[](size_t pos) const {
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

Vec4D ToVec4D(const Vec3D& vec_3d) {
  return {vec_3d.x, vec_3d.y, vec_3d.z, 1};
}
Vec3D ToVec3D(const Vec4D& vec_4d) {
  assert(std::abs(vec_4d.t) > Vec4D::kMinT);
  return {vec_4d.x / vec_4d.t, vec_4d.y / vec_4d.t, vec_4d.z / vec_4d.t};
}

Matrix Matrix::OxRotation(double angle) {
  return {{{{1, 0, 0, 0},
            {0, std::cos(angle), -std::sin(angle), 0},
            {0, std::sin(angle), std::cos(angle), 0},
            {0, 0, 0, 1}}}};
}

Matrix Matrix::OyRotation(double angle) {
  return {{{{std::cos(angle), 0, std::sin(angle), 0},
            {0, 1, 0, 0},
            {-std::sin(angle), 0, std::cos(angle), 0},
            {0, 0, 0, 1}}}};
}

Matrix Matrix::MoveMatrix(const Vec3D& vec_3d) {
  return {{{{1, 0, 0, vec_3d.x},
            {0, 1, 0, vec_3d.y},
            {0, 0, 1, vec_3d.z},
            {0, 0, 0, 1}}}};
}

Matrix Matrix::ProjectionMatrix(double near, double far) {
  return {{{{far, 0, 0, 0},
            {0, far, 0, 0},
            {0, 0, near + far, -near * far},
            {0, 0, 1, 0}}}};
}

Matrix Matrix::operator*(const Matrix& other) const {
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

Vec4D Matrix::operator*(const Vec4D& other) const {
  Vec4D result{};
  for (uint32_t i = 0; i < 4; ++i) {
    for (uint32_t j = 0; j < 4; ++j) {
      result[i] += array[i][j] * other[j];
    }
  }
  return result;
}

Vec3D Matrix::operator*(const Vec3D& other) const {
  return ToVec3D(*this * ToVec4D(other));
}

};  // namespace Renderer