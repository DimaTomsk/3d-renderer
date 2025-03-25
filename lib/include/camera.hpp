#pragma once
#include <cstdint>
#include <numbers>

#include "primitives.hpp"

namespace Renderer {

struct Camera {
  double near_{0.1};
  double far_{101};

  const double view_angle{std::numbers::pi_v<double> / 3};

  double ox_rotation{};
  double oy_rotation{};
  Vec3D pos{0, 0, -2};

  [[nodiscard]] double GetScreenSize() const {
    return far_ * std::tan(view_angle / 2) * 2;
  }

  [[nodiscard]] Matrix ProjectAndScale() const {
    return Matrix::MoveMatrix({
               GetScreenSize() / 2,
               GetScreenSize() / 2,
               0,
           }) *
           Matrix{{{{far_, 0, 0, 0},
                    {0, far_, 0, 0},
                    {0, 0, near_ + far_, -near_ * far_},
                    {0, 0, 1, 0}}}};
  }

  [[nodiscard]] Matrix Move() const {
    return Matrix::OxRotation(-ox_rotation) * Matrix::OyRotation(-oy_rotation) *
           Matrix::MoveMatrix(-pos);
  }
};

}  // namespace Renderer
