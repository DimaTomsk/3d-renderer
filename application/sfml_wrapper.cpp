#include "sfml_wrapper.hpp"

#include <iostream>
#include <numbers>  // std::numbers

namespace Renderer {
SFMLWrapper::SFMLWrapper(std::unique_ptr<Application> application,
                         std::unique_ptr<Renderer> renderer)
    : width_(renderer->width_),
      height_(renderer->height_),
      buffer_({width_, height_}, sf::Color(0, 0, 0)),
      application_(std::move(application)),
      renderer_(std::move(renderer)),
      window(sf::VideoMode({width_, height_}), std::string{kWindowName}) {}

void SFMLWrapper::InfinityLoop() {
  sf::Texture texture{buffer_};
  sf::Sprite bufferSprite(texture);

  const sf::Vector2i start_mouse_position = sf::Mouse::getPosition();

  Vec3D camera_move;
  static constexpr double speed = 0.01;

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      // "close requested" event: we close the window
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
          window.close();
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::W) {
          camera_move.z = speed;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::A) {
          camera_move.x = -speed;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::S) {
          camera_move.z = -speed;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::D) {
          camera_move.x = speed;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
          camera_move.y = -speed;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::LShift) {
          // camera_move.y = speed;
        }
      }
      if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
        if (keyReleased->scancode == sf::Keyboard::Scancode::W) {
          camera_move.z = 0;
        }
        if (keyReleased->scancode == sf::Keyboard::Scancode::A) {
          camera_move.x = 0;
        }
        if (keyReleased->scancode == sf::Keyboard::Scancode::S) {
          camera_move.z = 0;
        }
        if (keyReleased->scancode == sf::Keyboard::Scancode::D) {
          camera_move.x = 0;
        }
        if (keyReleased->scancode == sf::Keyboard::Scancode::Space) {
          camera_move.y = 0;
        }
        if (keyReleased->scancode == sf::Keyboard::Scancode::LShift) {
          camera_move.y = 0;
        }
      }
    }

    const sf::Vector2i mouse_position = sf::Mouse::getPosition();
    double oy_rotation =
        static_cast<double>(mouse_position.x - start_mouse_position.x) *
        std::numbers::pi_v<double> / static_cast<double>(width_);
    double ox_rotation =
        static_cast<double>(mouse_position.y - start_mouse_position.y) *
        std::numbers::pi_v<double> / static_cast<double>(height_);

    application_->SetAngles(-ox_rotation, oy_rotation);
    application_->MoveCamera(camera_move);

    UpdateBuffer(renderer_->Render(*application_));
    texture.update(buffer_);

    window.draw(bufferSprite);

    window.display();
  }
}

void SFMLWrapper::UpdateBuffer(const Frame& frame) {
  for (uint32_t i = 0; i < width_; ++i) {
    for (uint32_t j = 0; j < height_; ++j) {
      buffer_.setPixel({i, j}, ColorToSf(frame[i][j]));
    }
  }
}

sf::Color SFMLWrapper::ColorToSf(const Color& color) {
  return sf::Color{color.r, color.g, color.b};
}

}  // namespace Renderer
