#include <iostream>

#include "sfml_wrapper.hpp"

int main() {
  constexpr int32_t width = 800;
  constexpr int32_t height = 600;

  auto renderer = std::make_unique<Renderer::Renderer>(width, height);
  auto application = std::make_unique<Renderer::Application>();

  Renderer::Object teapot = Renderer::Object::FromFile("samples/teapot.off");

  application->AddObject(std::move(teapot));

  application->MoveCamera(Renderer::Vec3D{0, 0, -2});

  {

    Renderer::Point p1{{0, 1, 3}, Renderer::Color::Blue()};
    Renderer::Point p2{{1, 0, 3}, Renderer::Color::Blue()};
    Renderer::Point p3{{-1, 0, 3}, Renderer::Color::Blue()};
    Renderer::Point p4{{0, -1, 3}, Renderer::Color::Blue()};

    Renderer::Point p5{{0, 1, 5}, Renderer::Color::Green()};
    Renderer::Point p6{{1, 0, 5}, Renderer::Color::Green()};
    Renderer::Point p7{{-1, 0, 5}, Renderer::Color::Green()};
    Renderer::Point p8{{0, -1, 5}, Renderer::Color::Green()};

    std::vector points{p1, p2, p3, p4, p5, p6, p7, p8};
    std::vector<Renderer::Primitive> new_object{p1, p2, p3, p4, p5, p6, p7, p8};
    new_object.emplace_back(Renderer::Triangle{
        {{{0, 1, 3}, {1, 0, 3}, {-1, 0, 3}}}, Renderer::Color::Rand()});

    for (int i = 0; i < points.size(); ++i) {
      for (int j = i + 1; j + 1 < points.size(); ++j) {
        if (std::abs(points[i].points[0].x - points[j].points[0].x) +
                std::abs(points[i].points[0].y - points[j].points[0].y) +
                std::abs(points[i].points[0].z - points[j].points[0].z) ==
            2) {
          new_object.emplace_back(
              Renderer::Line{{{points[i].points[0], points[j].points[0]}},
                             Renderer::Color::Rand()});
        }
      }
    }

    application->AddObject(Renderer::Object{new_object});
  }
  Renderer::SFMLWrapper sfml_wrapper{std::move(application),
                                     std::move(renderer)};

  sfml_wrapper.InfinityLoop();
}
