#include "primitives.hpp"

namespace Renderer {
Color Color::Rand() {
  static std::mt19937 rnd;  // NOLINT(*-msc51-cpp)
  uint32_t value = rnd();
  return {static_cast<uint8_t>(value >> 0 & 0xff),
          static_cast<uint8_t>(rnd() >> 8 & 0xff),
          static_cast<uint8_t>(rnd() >> 16 & 0xff)};
}

}  // namespace Renderer