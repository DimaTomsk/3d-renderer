#include <iostream>

#include "sfml_wrapper.hpp"

int main() {
  constexpr int32_t width = 800;
  constexpr int32_t height = 600;

  auto renderer = std::make_unique<Renderer::Renderer>(width, height);
  auto application = std::make_unique<Renderer::Application>();

  Renderer::Object teapot = Renderer::Object::FromFile("samples/teapot.off");

  application->AddObject(std::move(teapot));

  Renderer::SFMLWrapper sfml_wrapper{std::move(application),
                                     std::move(renderer)};

  sfml_wrapper.InfinityLoop();
}
