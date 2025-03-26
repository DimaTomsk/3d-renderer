#pragma once

#include "camera.hpp"
#include "primitives.hpp"

#include <vector>

namespace Renderer {

struct Object {
  std::vector<Primitive> primitives_;

  static Object FromFile(const std::string& filename);
};

class Application {
 public:
  [[nodiscard]] std::vector<Object> GetObjects() const { return objects_; }

  [[nodiscard]] Camera GetCamera() const { return camera_; }

  void AddObject(Object object) { objects_.push_back(std::move(object)); }

  void SetAngles(double ox_rotation, double oy_rotation) {
    camera_.ox_rotation = ox_rotation;
    camera_.oy_rotation = oy_rotation;
  }

  void MoveCamera(const Vec3D& camera_move) {
    camera_.pos = camera_.pos + Matrix::OyRotation(camera_.oy_rotation) *
                                    Matrix::OxRotation(camera_.ox_rotation) *
                                    camera_move;
  }

 private:
  std::vector<Object> objects_;
  Camera camera_;
};
}  // namespace Renderer
