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

  [[nodiscard]] static double GetScreenSize();

  [[nodiscard]] static Matrix ProjectAndScale();

  [[nodiscard]] Matrix MoveObjects() const;
};

}  // namespace Renderer
