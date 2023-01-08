#ifndef VEC2_HPP
#define VEC2_HPP

#include <cmath>
#include <cstdlib>

// typedef std::tuple<unsigned int, unsigned int> vec2;

class vec2 {
public:
  unsigned int x;
  unsigned int y;
  vec2();
  vec2(unsigned int x, unsigned int y);
};

int get_dist(vec2 a, vec2 b);

#endif