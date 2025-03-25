#pragma once

#include "application.hpp"
#include "renderer.hpp"

#include "SFML/Graphics.hpp"

namespace Renderer {
class SFMLWrapper {
 public:
  SFMLWrapper(std::unique_ptr<Application> application,
              std::unique_ptr<Renderer> renderer);

  void InfinityLoop();
  void UpdateBuffer(const Frame& frame);

  static sf::Color ColorToSf(const Color& color) {
    return sf::Color{color.r, color.g, color.b};
  }

 private:
  static constexpr std::string_view kWindowName = "SFML Window";
  const uint32_t width_;
  const uint32_t height_;

  sf::Image buffer_;

  std::unique_ptr<Application> application_;
  std::unique_ptr<Renderer> renderer_;
  sf::RenderWindow window;
};
};  // namespace Renderer
