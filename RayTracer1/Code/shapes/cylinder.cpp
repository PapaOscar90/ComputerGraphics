#include "cylinder.h"

#include <cmath>

using namespace std;

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

  Dx2 = pow(ray.D.x, 2);
  Dy2 = pow(ray.D.y, 2);
  OxCx = ray.O.x - position.x;
  OyCy = ray.O.y - position.y;
  R2 = pow(radius,2);

  double a = Dx2 + Dy2;
  double b = 2*(ray.D.x * (OxCx) + ray.D.y * (OyCy));
  double c = pow(OxCx, 2) + pow(OyCy, 2) - R2;

  double descriminate = pow(b,2) - 4*a*c;

  if(descriminate <= 0){
    return Hit::NO_HIT();
  } else {
    t = min((b*-1) + sqrt(descriminate), (b*-1) - sqrt(descriminate));
    t /= (2*a);
  }


  N = 2 * (ray.at(t) - position);

  return Hit(t, N);
}

Cylinder::Cylinder(Point const &pos, Vector const direction, double const radius, double const height)
  : position(pos), direction(direction), radius(radius), height(height) {}
