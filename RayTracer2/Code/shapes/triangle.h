#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"
#include <array>

class Triangle : public Object {
public:
  Triangle(Point const &vertex1, Point const &vertex2, Point const &vertex3);

  virtual Hit intersect(Ray const &ray);

  std::array<Point, 3> const vertices;
};

#endif
