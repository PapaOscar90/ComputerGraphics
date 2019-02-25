#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "../object.h"

class Cylinder : public Object {
public:
  Cylinder(Point const &point, Vector const direction, double const radius);

  virtual Hit intersect(Ray const &ray);

  // The begining point of the cylindar
  Point const position;
  // The direction the cylindar goes
  Vector const direction;
  // Radius of the tube
  double const radius;
};

#endif