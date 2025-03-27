#include "renderer.hpp"

namespace Renderer {
BufferedFrame::BufferedFrame(uint32_t width, uint32_t height)
    : width_(width),
      height_(height),
      depth_(width, std::vector(height, kMaxDepth)),
      current_frame_(width, std::vector(height, kDefaultColor)) {}

Frame BufferedFrame::Pop() {
  Frame result{std::move(current_frame_)};
  current_frame_ = std::vector(width_, std::vector(height_, kDefaultColor));
  std::ranges::fill(depth_, std::vector(height_, kMaxDepth));
  return result;
}

void BufferedFrame::SetPixel(uint32_t depth, uint32_t x, uint32_t y,
                             Color color) {
  assert(x < width_);
  assert(y < height_);

  if (depth_[x][y] <= depth) {
    return;
  }
  depth_[x][y] = depth;
  current_frame_[x][y] = color;
}

Color BufferedFrame::GetPixel(uint32_t x, uint32_t y) const {
  assert(x < width_);
  assert(y < height_);
  return current_frame_[x][y];
}

Renderer::Renderer(int32_t width, int32_t height)
    : width_(width), height_(height), buffered_frame_(width, height) {}

Frame Renderer::Render(const Application& application) {
  const auto camera = application.GetCamera();

  for (Object& object : application.GetObjects()) {
    for (Primitive& obj_primitive : object.primitives_) {
      std::visit(
          [&camera, this]<typename T>(T& primitive) {
            MoveCoordinates(primitive, camera);

            for (auto base_primitive : Clipping(primitive)) {
              for (auto scaled_primitive : ProjectAndScale(base_primitive)) {
                if constexpr (std::is_same_v<T, Point>) {
                  RenderPoint(scaled_primitive);
                } else if constexpr (std::is_same_v<T, Line>) {
                  RenderLine(scaled_primitive);
                } else if constexpr (std::is_same_v<T, Triangle>) {
                  RenderTriangle(scaled_primitive);
                } else {
                  static_assert(false, "Unexpected primitive type");
                }
              }
            }
          },
          obj_primitive);
    }
  }
  return buffered_frame_.Pop();
}

void Renderer::RenderPoint(const Point& point) {
  Vec3Di coords = WorldToFrameCoords(point[0]);

  if (!(0 <= coords.x and coords.x < width_)) {
    return;
  }
  if (!(0 <= coords.y and coords.y < height_)) {
    return;
  }
  buffered_frame_.SetPixel(coords.z, coords.x, coords.y, point.color);
}

void Renderer::RenderLine(const Line& line) {
  Vec3Di a = WorldToFrameCoords(line[0]);
  Vec3Di b = WorldToFrameCoords(line[1]);

  std::vector<Vec3Di> rasterization = RasterizeLine(a, b);

  for (const auto& [x, y, z] : rasterization) {
    if (0 <= x and x < width_) {
      if (0 <= y and y < height_) {
        buffered_frame_.SetPixel(z, x, y, line.color);
      }
    }
  }
}

void Renderer::RenderTriangle(const Triangle& triangle) {

  Vec3Di a = WorldToFrameCoords(triangle[0]);
  Vec3Di b = WorldToFrameCoords(triangle[1]);
  Vec3Di c = WorldToFrameCoords(triangle[2]);

  auto u = RasterizeLine(a, b);
  auto v = RasterizeLine(b, c);
  auto t = RasterizeLine(a, c);

  using VecCmp = decltype([](const Vec3Di& first, const Vec3Di& second) {
    return first.x < second.x;
  });

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

Vec3Di Renderer::WorldToFrameCoords(const Vec3D& pos) const {
  const double full_screen = Camera::GetScreenSize();
  const int32_t offset = (width_ - height_) / 2;

  const auto x = static_cast<int32_t>(pos.x * width_ / full_screen);
  const auto y = static_cast<int32_t>(pos.y * width_ / full_screen);

  const double ratio = (pos.z - Camera::kNear) / (Camera::kFar - Camera::kNear);
  const auto z = static_cast<int32_t>(ratio * BufferedFrame::kMaxDepth);

  return {x, y - offset, z};
}

std::vector<Vec3Di> Renderer::RasterizeLine(Vec3Di a, Vec3Di b) {
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

}  // namespace Renderer