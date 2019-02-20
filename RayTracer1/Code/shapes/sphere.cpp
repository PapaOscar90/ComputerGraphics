#include "sphere.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray) {
  /* *********************************************************************
   * RT1.1: INTERSECTION CALCULATION
   *
   * Given: ray, position, r     * Sought: intersects? if true: *t
   *
   * Insert calculation of ray/sphere intersection here.
   *
   * You have the sphere's center (C) and radius (r) as well as
   * the ray's origin (ray.O) and direction (ray.D).
   *
   * Computation for Intersection:
   * If the ray does not intersect the sphere, return NoHit.
   * Otherwise, return Hit and place the distance of the
   * intersection point from the ray origin in *t (see example).
   *  (e+td-c)*(e+td-c) - R^2 = 0
   * Where e is the origin, t the distance, d the vector of the ray
   * and c the center of the sphere
   *
   * This expands into the quadratic solution form, which looks at the
   * discriminant. Specifically if:
   *
   * D = (d*(e-c))^2 - (d*d)((e-c)*(e-c)- R^2)
   *                           = 0 -> single solution, ray grazes
   *                           > 0 -> Two strikes, get closest
   *                           < 0 -> The ray misses the sphere
   *
   * The solution for t is:
   *
   *  t = (-d*(e-c) +/- sqrt(D)) / (d*d)
   ***************************************************************************/
  double t;
  double d2 = ray.D.dot(ray.D);
  double r2 = r * r;
  Vector eMinusC = ray.O - position;

  double discriminant =
      (pow(ray.D.dot(eMinusC), 2)) - d2 * ((eMinusC.dot(eMinusC)) - r2);

  if (discriminant < 0.) {
    return Hit::NO_HIT();
  } else if (discriminant == 0.) {
    // One hit
    t = (-ray.D.dot(eMinusC)) / d2;
  } else {
    // Two hits, needs to find closest
    t = -ray.D.dot(eMinusC);
    t = min(t + sqrt(discriminant), t - sqrt(discriminant));
    t /= d2;
  }

  /**************************************************************************
   * RT1.2: NORMAL CALCULATION
   *
   * Given: t, C, r
   * Sought: N
   *
   * The normal at point p = (e+td) on the surface, is given by:
   *
   * N = 2(p-c)
   *
   **************************************************************************/
  Vector N = 2 * (ray.at(t) - position);

  return Hit(t, N);
}

Sphere::Sphere(Point const &pos, double radius) : position(pos), r(radius) {}
