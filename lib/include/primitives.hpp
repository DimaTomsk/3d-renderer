#pragma once
#include <array>
#include <cstdint>
#include <variant>

#include "matrix.hpp"

#include <random>

namespace Renderer {
struct Color {
  uint8_t r{};
  uint8_t g{};
  uint8_t b{};

  static constexpr Color Black() { return {0, 0, 0}; }

  static constexpr Color Red() { return {255, 0, 0}; }

  static constexpr Color Green() { return {0, 255, 0}; }

  static constexpr Color Blue() { return {0, 0, 255}; }

  static Color Rand() {
    static std::mt19937 rnd;  // NOLINT(*-msc51-cpp)
    uint32_t value = rnd();
    return {static_cast<uint8_t>(value >> 0 & 0xff),
            static_cast<uint8_t>(rnd() >> 8 & 255),
            static_cast<uint8_t>(rnd() >> 16 & 255)};
  }
};

template <uint8_t N>
struct BasePrimitive {
  std::array<Vec3D, N> points;
  Color color;

  const Vec3D& operator[](uint8_t pos) const {
    if (pos >= N) {
      throw std::runtime_error("Index out of bound");
    }
    return points[pos];
  }

  Vec3D& operator[](uint8_t pos) {
    if (pos >= N) {
      throw std::runtime_error("Index out of bound");
    }
    return points[pos];
  }
};

using Point = BasePrimitive<1>;
using Line = BasePrimitive<2>;
using Triangle = BasePrimitive<3>;

using Primitive = std::variant<Point, Line, Triangle>;

}  // namespace Renderer
