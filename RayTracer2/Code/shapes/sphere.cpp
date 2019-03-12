#include "sphere.h"
#include "solvers.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray) {
  // Sphere formula: ||x - position||^2 = r^2
  // Line formula:   x = ray.O + t * ray.D

  Vector L = ray.O - position;
  double a = ray.D.dot(ray.D);
  double b = 2 * ray.D.dot(L);
  double c = L.dot(L) - r * r;

  double t0;
  double t1;
  if (!Solvers::quadratic(a, b, c, t0, t1))
    return Hit::NO_HIT();

  // t0 is closest hit
  if (t0 < 0) // check if it is not behind the camera
  {
    t0 = t1;    // try t1
    if (t0 < 0) // both behind the camera
      return Hit::NO_HIT();
  }

  // calculate normal
  Point hit = ray.at(t0);
  Vector N = (hit - position).normalized();

  // determine orientation of the normal
  if (N.dot(ray.D) > 0)
    N = -N;

  return Hit(t0, N);
}

// Rotate the ray around an axis, a certain angle vi
Triple rotate(Triple const &v, double const angle, Vector const &k) {
  return v * cos(angle) + (k.cross(v) * sin(angle)) +
         k * (k.dot(v)) * (1 - cos(angle));
}

// Find the texture coordinate u,v of a hit on the sphere
TextureCoordinates Sphere::textureCoordinates(Point const &point) {
  Vector hitVector = point - position;
  hitVector = rotate(hitVector, angle, axis);

  TextureCoordinates hitCoordinates;
  hitCoordinates.u = (M_PI + atan2(-hitVector.y, -hitVector.x)) / (2 * M_PI);
  hitCoordinates.v = acos(hitVector.z / r) / M_PI;

  return hitCoordinates;
}

Sphere::Sphere(Point const &pos, double radius) : position(pos), r(radius) {}
