#include "camera.hpp"

namespace Renderer {
double Camera::GetScreenSize() {
  return kFar * std::tan(kViewAngle / 2) * 2;
}

Matrix Camera::ProjectAndScale() {
  return Matrix::MoveMatrix({
             GetScreenSize() / 2,
             GetScreenSize() / 2,
             0,
         }) *
         Matrix::ProjectionMatrix(kNear, kFar);
}

Matrix Camera::MoveObjects() const {
  return Matrix::OxRotation(-ox_rotation) * Matrix::OyRotation(-oy_rotation) *
         Matrix::MoveMatrix(-pos);
}

}  // namespace Renderer
