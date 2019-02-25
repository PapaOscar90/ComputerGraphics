#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "../object.h"

class Cylinder : public Object {
public:
  Cylinder(Point const &pointA, Point const &pointB, double const radius);

  virtual Hit intersect(Ray const &ray);

  // The cylinder is defined between two points with a radius
  Point const pointA;
  Point const pointB;
  double const radius;
};

#endif
