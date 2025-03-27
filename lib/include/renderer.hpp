#pragma once

#include <cassert>
#include <vector>

#include "application.hpp"
#include "primitives.hpp"

#include <map>
#include <set>

namespace Renderer {

using Frame = std::vector<std::vector<Color>>;

class BufferedFrame {
  static constexpr Color kDefaultColor = Color::Black();

 public:
  static constexpr uint32_t kMaxDepth = 100'000;

  BufferedFrame(uint32_t width, uint32_t height);

  Frame Pop();

  void SetPixel(uint32_t depth, uint32_t x, uint32_t y, Color color);
  [[nodiscard]] Color GetPixel(uint32_t x, uint32_t y) const;

 private:
  const uint32_t width_;
  const uint32_t height_;

  std::vector<std::vector<uint32_t>> depth_;
  Frame current_frame_;
};

class Renderer {
 public:
  Renderer(int32_t width, int32_t height);

  Frame Render(const Application& application);

  const int32_t width_;
  const int32_t height_;

 private:
  BufferedFrame buffered_frame_;

  template <uint8_t Points>
  void MoveCoordinates(BasePrimitive<Points>& primitive, const Camera& camera) {
    for (auto& point : primitive.points) {
      point = camera.MoveObjects() * point;
    }
  }

  template <uint8_t Points>
  std::vector<BasePrimitive<Points>> Clipping(
      const BasePrimitive<Points>& primitive) {
    std::vector<BasePrimitive<Points>> result;
    std::vector<BasePrimitive<Points>> to_clip{primitive};

    while (!to_clip.empty()) {
      auto current_primitive = to_clip.back();
      to_clip.pop_back();

      if (std::all_of(current_primitive.points.begin(),
                      current_primitive.points.end(),
                      [](Vec3D vec) { return vec.z >= Camera::kNear; })) {
        result.push_back(current_primitive);
        continue;
      }

      if (std::all_of(current_primitive.points.begin(),
                      current_primitive.points.end(),
                      [](Vec3D vec) { return vec.z <= Camera::kNear; })) {
        continue;
      }

      if (Points == 1) {  // No clipping for points, just drop or take it
        continue;
      }

      for (size_t i = 0; i < Points; ++i) {
        if (current_primitive[i].z < Camera::kNear) {
          Vec3D next_point = current_primitive[(i + 1) % Points];
          Vec3D bad_point = current_primitive[i];
          Vec3D prev_point = current_primitive[(i + Points - 1) % Points];

          if (next_point.z <= prev_point.z) {
            std::swap(next_point, prev_point);
          }

          if (next_point.z <= Camera::kNear) {
            continue;
          }

          double dz1 = Camera::kNear - bad_point.z;
          double dz2 = next_point.z - Camera::kNear;

          if (std::abs(dz1 + dz2) < Vec4D::kMinT) {
            continue;
          }

          Vec3D new_point;
          new_point.x = (bad_point.x * dz2 + next_point.x * dz1) / (dz1 + dz2);
          new_point.y = (bad_point.y * dz2 + next_point.y * dz1) / (dz1 + dz2);
          new_point.z = Camera::kNear;

          if constexpr (Points == 2) {
            to_clip.push_back(Line{{new_point, next_point}, primitive.color});
          } else if constexpr (Points == 3) {
            to_clip.push_back(
                Triangle{{new_point, next_point, prev_point}, primitive.color});
            to_clip.push_back(
                Triangle{{new_point, bad_point, prev_point}, primitive.color});
          } else {
          }
        }
      }
    }

    return result;
  }

  template <uint8_t Points>
  std::vector<BasePrimitive<Points>> ProjectAndScale(
      BasePrimitive<Points>& primitive) {
    for (auto& point : primitive.points) {
      point = Camera::ProjectAndScale() * point;
    }

    if (std::all_of(primitive.points.begin(), primitive.points.end(),
                    [](const Vec3D& vec) { return vec.y < 0; }) ||
        std::all_of(primitive.points.begin(), primitive.points.end(),
                    [](const Vec3D& vec) { return vec.x < 0; }) ||
        std::all_of(
            primitive.points.begin(), primitive.points.end(),
            [](const Vec3D& vec) { return vec.x > Camera::GetScreenSize(); }) ||
        std::all_of(
            primitive.points.begin(), primitive.points.end(),
            [](const Vec3D& vec) { return vec.y > Camera::GetScreenSize(); })) {
      return {};
    }
    return {primitive};
  }

  void RenderPoint(const Point& point);

  void RenderLine(const Line& line);

  void RenderTriangle(const Triangle& triangle);

  [[nodiscard]] Vec3Di WorldToFrameCoords(const Vec3D& pos) const;

  [[nodiscard]] static std::vector<Vec3Di> RasterizeLine(Vec3Di a, Vec3Di b);
};
}  // namespace Renderer
