#include "cylinder.h"

#include <cmath>

Hit Cylinder::intersect(Ray const &ray) {
  /* ******************************************************************
   * Intersection Calcuation
   * 
   * Given: ray, position, direction, radius
   * Sough: intersect? if true: *t,N
   * 
   * Ray's Origin (ray.O), Ray's direction (ray.D),
   * cylindar position (C), cylindar direction+ (V), radius(R)
   * 
   * Computation:
   * The cylindar formula is derived similarly to the sphere,
   * so we will leave the verbose definitions to the reader
   * if they are interested. The intersection is determined
   * by the determinate. It informs us if the cylindar is intersected
   * 
   * Implicitly: (Dx^2 + Dy^2)t^2 + 2(OxDx + OyDy)t + (Ox^2 + Oy^2 - R^2) = 0
   * where Oi = ray.O.i - C.i
   * 
   * so:
   * a =  (Dx^2 + Dy^2)
   * b = 2(Dx*(ray.O.x - C.x) + Dy*(ray.O.y - C.y))
   * c =  ((ray.O.x - C.x)^2 + (ray.O.y - C.y)^2 - R^2)
   * 
   * D = b^2 - 4ac
   * 
   * t = (-b +/- sqrt(D)) / 2a
   * 
   *******************************************************************/ 
  double t;
  double Dx2, Dy2, OxCx, OyCy, R2;
  Vector N;


  return Hit(t, N);
}

Cylinder::Cylinder(Point const &pos, Vector const direction, double const radius)
  : position(pos), direction(direction), radius(radius) {}
