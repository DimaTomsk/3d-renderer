#include "application.hpp"

#include "fstream"

namespace Renderer {
Object Object::FromFile(const std::string& filename) {
  Object result;
  std::ifstream file(filename);

  std::string format;
  file >> format;
  assert(format == "OFF");

  uint32_t n, m, k;
  file >> n >> m >> k;
  std::vector<Vec3D> points;
  points.resize(n);
  for (size_t i = 0; i < n; ++i) {
    file >> points[i].x >> points[i].y >> points[i].z;
  }

  for (size_t j = 0; j < m; ++j) {
    uint32_t v;
    file >> v;
    std::vector<uint32_t> ids(v);
    for (uint32_t& x : ids) {
      file >> x;
    }

    auto color = Color::Rand();

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

}  // namespace Renderer