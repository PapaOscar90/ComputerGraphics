#include "plane.h"

#include <cmath>

Hit Plane::intersect(Ray const &ray) {
  /* Your intersect calculation goes here */
  bool hit false;

  double t = 0 /* = ... */;
  Vector N /* = ... */;

  return Hit(t, N);
}

Plane::Plane(/* YOUR DATAMEMBERS HERE */)
//:
// See sphere.cpp how to initialize your data members
{}