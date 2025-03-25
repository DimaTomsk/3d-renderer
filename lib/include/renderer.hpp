#pragma once

#include <cassert>
#include <vector>

#include "application.hpp"
#include "primitives.hpp"

#include <map>
#include <set>

namespace Renderer {

struct Frame {
  std::vector<std::vector<Color>> pixels_;

  [[nodiscard]] size_t Width() const { return pixels_.size(); }
  [[nodiscard]] size_t Height() const { return pixels_[0].size(); }
};

class BufferedFrame {
  static constexpr Color kDefaultColor = Color::Black();

 public:
  static constexpr uint32_t kMaxDepth = 100000;

  BufferedFrame(uint32_t width, uint32_t height)
      : width_(width),
        height_(height),
        depth_(width, std::vector(height, kMaxDepth)),
        color_(width, std::vector(height, kDefaultColor)) {}

  Frame Pop() {
    Frame result{std::move(color_)};
    color_ = std::vector(width_, std::vector(height_, kDefaultColor));
    std::ranges::fill(depth_, std::vector(height_, kMaxDepth));
    return result;
  }

  void SetPixel(uint32_t depth, uint32_t x, uint32_t y, Color color) {
    assert(x < width_);
    assert(y < height_);

    if (depth_[x][y] <= depth) {
      return;
    }
    depth_[x][y] = depth;
    color_[x][y] = color;
  }

  Color GetPixel(uint32_t x, uint32_t y) {
    assert(x < width_);
    assert(y < height_);
    return color_[x][y];
  }

 private:
  const uint32_t width_;
  const uint32_t height_;

  std::vector<std::vector<uint32_t>> depth_;
  std::vector<std::vector<Color>> color_;
};

class Renderer {
 public:
  Renderer(int32_t width, int32_t height)
      : width_(width), height_(height), buffered_frame_(width, height) {}

  Frame Render(const Application& application) {
    const auto camera = application.GetCamera();

    for (const auto& object : application.GetObjects()) {
      for (const auto& primitive : object.primitives_) {
        std::visit(
            [&camera, this]<typename T>(T primitive) {
              MoveCoordinates(primitive, camera);

              using T0 = std::decay_t<T>;

              for (auto base_primitive : Clipping(primitive, camera)) {
                for (auto scaled_primitive :
                     ProjectAndScale(base_primitive, camera)) {
                  if constexpr (std::is_same_v<T0, Point>) {
                    RenderPoint(camera, scaled_primitive);
                  } else if constexpr (std::is_same_v<T0, Line>) {
                    RenderLine(camera, scaled_primitive);
                  } else if constexpr (std::is_same_v<T0, Triangle>) {
                    RenderTriangle(camera, scaled_primitive);
                  } else {
                    static_assert(false, "Unexpected primitive type");
                  }
                }
              }
            },
            primitive);
      }
    }
    return buffered_frame_.Pop();
  }

  const int32_t width_;
  const int32_t height_;

 private:
  BufferedFrame buffered_frame_;

  template <uint8_t Points>
  void MoveCoordinates(BasePrimitive<Points>& primitive, const Camera& camera) {
    for (auto& point : primitive.points) {
      point = camera.Move() * point;
    }
  }

  template <uint8_t Points>
  std::vector<BasePrimitive<Points>> Clipping(BasePrimitive<Points>& primitive,
                                              const Camera& camera) {
    std::vector<BasePrimitive<Points>> result;
    std::vector<BasePrimitive<Points>> to_clip{primitive};

    while (!to_clip.empty()) {
      auto current_primitive = to_clip.back();
      to_clip.pop_back();

      if (std::all_of(current_primitive.points.begin(),
                      current_primitive.points.end(),
                      [&camera](Vec3D vec) { return vec.z >= camera.near_; })) {
        result.push_back(current_primitive);
        continue;
      }

      if (std::all_of(current_primitive.points.begin(),
                      current_primitive.points.end(),
                      [&camera](Vec3D vec) { return vec.z <= camera.near_; })) {
        continue;
      }

      if (Points == 1) {  // No clipping for points, just drop or take it
        continue;
      }

      for (size_t i = 0; i < Points; ++i) {
        if (current_primitive[i].z < camera.near_) {
          Vec3D next_point = current_primitive[(i + 1) % Points];
          Vec3D bad_point = current_primitive[i];
          Vec3D prev_point = current_primitive[(i + Points - 1) % Points];

          if (next_point.z <= prev_point.z) {
            std::swap(next_point, prev_point);
          }

          if (next_point.z <= camera.near_) {
            continue;
          }

          double dz1 = camera.near_ - bad_point.z;
          double dz2 = next_point.z - camera.near_;

          if (std::abs(dz1 + dz2) < Vec4D::kMinT) {
            continue;
          }

          Vec3D new_point;
          new_point.x = (bad_point.x * dz2 + next_point.x * dz1) / (dz1 + dz2);
          new_point.y = (bad_point.y * dz2 + next_point.y * dz1) / (dz1 + dz2);
          new_point.z = camera.near_;

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
      BasePrimitive<Points>& primitive, const Camera& camera) {
    for (auto& point : primitive.points) {
      point = camera.ProjectAndScale() * point;
    }

    if (std::all_of(primitive.points.begin(), primitive.points.end(),
                    [](const Vec3D& vec) { return vec.y < 0; }) ||
        std::all_of(primitive.points.begin(), primitive.points.end(),
                    [](const Vec3D& vec) { return vec.x < 0; }) ||
        std::all_of(primitive.points.begin(), primitive.points.end(),
                    [&camera](const Vec3D& vec) {
                      return vec.x > camera.GetScreenSize();
                    }) ||
        std::all_of(primitive.points.begin(), primitive.points.end(),
                    [&camera](const Vec3D& vec) {
                      return vec.y > camera.GetScreenSize();
                    })) {
      return {};
    }
    return {primitive};
  }

  void RenderPoint(const Camera& camera, Point point) {
    Vec3Di coords = WorldToFrameCoords(point[0], camera);

    if (!(0 <= coords.x and coords.x < width_)) {
      return;
    }
    if (!(0 <= coords.y and coords.y < height_)) {
      return;
    }
    buffered_frame_.SetPixel(coords.z, coords.x, coords.y, point.color);
  }

  std::vector<Vec3Di> RasterizeLine(Vec3Di a, Vec3Di b) {
    std::vector<Vec3Di> result;
    if (a.x == b.x and a.y == b.y) {
      return {{a}};
    }
    if (std::abs(a.x - b.x) > std::abs(a.y - b.y)) {
      if (a.x > b.x) {
        std::swap(a, b);
      }
      for (int32_t x = a.x; x <= b.x; ++x) {
        const auto y = static_cast<int32_t>(
            (x - a.x) * 1.0 / (b.x - a.x) * (b.y - a.y) + a.y);
        const auto z = static_cast<int32_t>(
            (x - a.x) * 1.0 / (b.x - a.x) * (b.z - a.z) + a.z);
        result.push_back({x, y, z});
      }
    } else {
      if (a.y > b.y) {
        std::swap(a, b);
      }
      for (int32_t y = a.y; y <= b.y; ++y) {
        const auto x = static_cast<int32_t>(
            (y - a.y) * 1.0 / (b.y - a.y) * (b.x - a.x) + a.x);
        const auto z = static_cast<int32_t>(
            (y - a.y) * 1.0 / (b.y - a.y) * (b.z - a.z) + a.z);

        result.push_back({x, y, z});
      }
    }
    return result;
  }

  void RenderLine(const Camera& camera, Line line) {
    Vec3Di a = WorldToFrameCoords(line.points[0], camera);
    Vec3Di b = WorldToFrameCoords(line.points[1], camera);

    std::vector<Vec3Di> rasterization = RasterizeLine(a, b);

    for (const auto& [x, y, z] : rasterization) {
      if (0 <= x and x < width_) {
        if (0 <= y and y < height_) {
          buffered_frame_.SetPixel(z, x, y, line.color);
        }
      }
    }
  }

  void RenderTriangle(const Camera& camera, Triangle triangle) {
    Vec3Di a = WorldToFrameCoords(triangle[0], camera);
    Vec3Di b = WorldToFrameCoords(triangle[1], camera);
    Vec3Di c = WorldToFrameCoords(triangle[2], camera);

    auto u = RasterizeLine(a, b);
    auto v = RasterizeLine(b, c);
    auto t = RasterizeLine(a, c);

    using VecCmp =
        decltype([](const Vec3Di& a, const Vec3Di& b) { return a.x < b.x; });

    std::unordered_map<int32_t, std::set<Vec3Di, VecCmp>> points;

    for (const auto& line : {u, v, t}) {
      for (const auto& point : line) {
        points[point.y].insert(point);
      }
    }

    const int32_t min_y = std::min({a.y, b.y, c.y});
    const int32_t max_y = std::max({a.y, b.y, c.y});

    for (int32_t y = std::max(0, min_y); y < height_ && y < max_y; ++y) {
      if (points[y].empty()) {
        continue;
      }

      const auto left = *points[y].begin();
      const auto right = *points[y].rbegin();

      for (int32_t x = std::max(0, left.x); x <= right.x && x < width_; ++x) {
        int32_t z = left.z;
        if (left.x != right.x) {
          z += (x - left.x) * (right.z - left.z) / (right.x - left.x);
        }
        buffered_frame_.SetPixel(z, x, y, triangle.color);
      }
    }
  }

  [[nodiscard]] Vec3Di WorldToFrameCoords(Vec3D pos,
                                          const Camera& camera) const {
    const double full_screen = camera.GetScreenSize();
    const int32_t offset =
        (static_cast<int32_t>(width_) - static_cast<int32_t>(height_)) / 2;

    const auto x = static_cast<int32_t>(pos.x * width_ / full_screen);
    const auto y = static_cast<int32_t>(pos.y * width_ / full_screen);

    const double ratio = (pos.z - camera.near_) / (camera.far_ - camera.near_);
    const auto z = static_cast<int32_t>(ratio * BufferedFrame::kMaxDepth);

    return {x, y - offset, z};
  }
};
}  // namespace Renderer
