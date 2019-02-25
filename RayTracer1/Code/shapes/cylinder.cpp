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
   * Implicitly: (ca.ca - (ca.rd * ca.rd))t^2 + 2(ca.ca*oc.rd - ca.oc*ca.rd)t + (ca.ca*oc.oc - ca.oc*ca.oc - r^2*ca.ca) = 0
   * where:
   * ca = pb-pa  // The direction to the end of cylinder
   * oc = ro-pa  // The direction to the cylinder
   * caca = ca.ca
   * card = ca.rd
   * caoc = ca.oc
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
  double card = ca.dot(rd);
  double caoc = ca.dot(oc);

  double a = caca - card*card;
  double b = (caca * oc.dot(ray.O)) - (caoc * card);
  double c = (caca * (oc.dot(oc))) - (caoc * caoc) - (pow(radius, 2) * caca);

  double det = pow(b,2) - (a*c);

  if(det < 0.0){
    return Hit::NO_HIT;
  } else {
    t = min(-b + sqrt(det), -b - sqrt(det));
    t /= 2*a;
  }

  N = 2 * (ray.at(t) - oc);
  return Hit(t, N);
}

Cylinder::Cylinder(Point const &pointA, Point const &pointB, double const radius)
  : pointA(pointA), pointB(pointB), radius(radius) {}
