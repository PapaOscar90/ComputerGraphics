#ifndef PLANE_H_
#define PLANE_H_

#include "../object.h"

class Plane : public Object {
public:
  Plane(Point const &pos, Vector const n);

  virtual Hit intersect(Ray const &ray);

  Point const position;
  Vector const n;
};

#endif