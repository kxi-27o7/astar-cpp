#include "vec2.hpp"

vec2::vec2() {
  x = 0;
  y = 0;
}
vec2::vec2(unsigned int __x, unsigned int __y) {
  x = __x;
  y = __y;
}

int get_dist(vec2 a, vec2 b) {
  return std::abs(std::sqrt(std::pow((float)a.x - (float)b.x, 2) +
                            std::pow((float)a.y - (float)b.y, 2)));
}
