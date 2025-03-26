#pragma once
#include <numbers>

#include "primitives.hpp"

namespace Renderer {

struct Camera {
  static constexpr double kNear{0.1};
  static constexpr double kFar{101};
  static constexpr double kViewAngle{std::numbers::pi_v<double> / 3};

  double ox_rotation{};
  double oy_rotation{};
  Vec3D pos{};

  [[nodiscard]] static double GetScreenSize() {
    return kFar * std::tan(kViewAngle / 2) * 2;
  }

  [[nodiscard]] static Matrix ProjectAndScale() {
    return Matrix::MoveMatrix({
               GetScreenSize() / 2,
               GetScreenSize() / 2,
               0,
           }) *
           Matrix::ProjectionMatrix(kNear, kFar);
  }

  [[nodiscard]] Matrix Move() const {
    return Matrix::OxRotation(-ox_rotation) * Matrix::OyRotation(-oy_rotation) *
           Matrix::MoveMatrix(-pos);
  }
};

}  // namespace Renderer
