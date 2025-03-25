#pragma once

#include "camera.hpp"

#include <vector>
#include "primitives.hpp"

namespace Renderer {

struct Object {
  std::vector<Primitive> primitives_;

  static Object FromFile(const std::string& filename);
};

class Application {
 public:
  [[nodiscard]] const std::vector<Object>& GetObjects() const {
    return objects_;
  }

  [[nodiscard]] Camera GetCamera() const { return camera_; }

  void AddObject(Object object) { objects_.push_back(std::move(object)); }

  void SetAngles(double ox_rotation, double oy_rotation) {
    camera_.ox_rotation = ox_rotation;
    camera_.oy_rotation = oy_rotation;
  }

  void MoveCamera(const Vec3D& camera_move) {
    camera_.pos =
        camera_.pos +
        ToVec3D(Matrix::OyRotation(camera_.oy_rotation) *
                Matrix::OxRotation(camera_.ox_rotation) * ToVec4D(camera_move));
  }

 private:
  std::vector<Object> objects_;
  Camera camera_;
};
}  // namespace Renderer
