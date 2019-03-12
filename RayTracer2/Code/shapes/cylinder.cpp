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
   * cylindar position start (pointA), cylindar position end (pointB), radius(R)
   *
   * Computation:
   * The cylindar formula is derived similarly to the sphere,
   * so we will leave the verbose definitions to the reader
   * if they are interested. The intersection is determined
   * by the determinate. It informs us if the cylindar is intersected
   *
   * Implicitly: (ca.ca - (ca.rd * ca.rd))t^2 + 2(ca.ca*oc.rd - ca.oc*ca.rd)t +
   *(ca.ca*oc.oc - ca.oc*ca.oc - r^2*ca.ca) = 0
   * where: ca = pb-pa  | The direction to the end of cylinder
   *        oc = ro-pa  | The direction to the cylinder
   *      caca = ca.ca  | Shorthands for later...
   *      card = ca.rd
   *      caoc = ca.oc
   *
   * a = caca - card*card
   * b = caca*(oc.rd) - caoc*card
   * c = caca*(oc.oc) - caoc*caoc - (r^2*caca)
   *
   * D = b^2 - 4ac
   *
   * t = (-b +/- sqrt(D)) / 2a
   *
   *******************************************************************/
  double t;
  Vector N;

  Vector ca = pointB - pointA;
  Vector oc = ray.O - pointA;

  double caca = ca.dot(ca);
  double card = ca.dot(ray.D);
  double caoc = ca.dot(oc);

  double a = caca - card * card;
  double b = (caca * oc.dot(ray.D)) - (caoc * card);
  double c = (caca * (oc.dot(oc))) - (caoc * caoc) - (pow(radius, 2) * caca);

  double det = pow(b, 2) - (a * c);

  if (det < 0.0) {
    return Hit::NO_HIT();
  }

  t = (-b - sqrt(det)) / a;

  double temp = caoc + t * card;

  if (temp > 0.0 && temp < caca) {
    return Hit(t, (oc + (t * ray.D) - ((ca * temp) / caca)) / radius);
  }

  if (temp < 0.0) {
    t = (0.0 - caoc) / card;
  } else {
    t = (caca - caoc) / card;
  }

  if (abs(b + a * t) < sqrt(det)) {
    return Hit(t, (ca * copysign(1, temp)) / caca);
  }

  return Hit::NO_HIT();
}

TextureCoordinates Cylinder::textureCoordinates(Point const &point) {
  throw std::logic_error("Not implemented.");
}

Cylinder::Cylinder(Point const &pointA, Point const &pointB,
                   double const radius)
    : pointA(pointA), pointB(pointB), radius(radius) {}
