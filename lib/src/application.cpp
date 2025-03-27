#include "application.hpp"

#include "fstream"

namespace Renderer {
Object Object::FromFile(const std::string& filename) {
  Object result;
  std::ifstream file(filename);

  std::string format;
  file >> format;

  if (format != "OFF") {
    throw std::runtime_error("Unsupported file format");
  }

  uint32_t n, m, k;
  file >> n >> m >> k;
  std::vector<Vec3D> points;
  points.resize(n);
  for (size_t i = 0; i < n; ++i) {
    file >> points[i].x >> points[i].z >> points[i].y;
    points[i].y *= -1;
  }

  for (size_t j = 0; j < m; ++j) {
    uint32_t v;
    file >> v;
    std::vector<uint32_t> ids(v);
    for (uint32_t& x : ids) {
      file >> x;
    }

    const auto color = Color::Rand();

    if (v == 1) {
      result.primitives_.emplace_back(Point{{points[ids[0]]}, color});
    } else if (v == 2) {
      result.primitives_.emplace_back(
          Line{{points[ids[0]], points[ids[1]]}, Color::Rand()});
    } else if (v == 3) {
      result.primitives_.emplace_back(
          Triangle{{points[ids[0]], points[ids[1]], points[ids[2]]}, color});
    } else if (v == 4) {
      result.primitives_.emplace_back(
          Triangle{{points[ids[0]], points[ids[1]], points[ids[2]]}, color});
      result.primitives_.emplace_back(
          Triangle{{points[ids[2]], points[ids[3]], points[ids[0]]}, color});
    } else {
      throw std::runtime_error("Unsupported primitives size: " +
                               std::to_string(v));
    }
  }

  return result;
}

void Application::SetAngles(double ox_rotation, double oy_rotation) {
  camera_.ox_rotation = ox_rotation;
  camera_.oy_rotation = oy_rotation;
}

void Application::MoveCamera(const Vec3D& camera_move) {
  camera_.pos = camera_.pos + Matrix::OyRotation(camera_.oy_rotation) *
                                  Matrix::OxRotation(camera_.ox_rotation) *
                                  camera_move;
}

}  // namespace Renderer