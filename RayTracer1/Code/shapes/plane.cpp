#include "plane.h"

#include <cmath>
#include <limits>

using namespace std;

const double EPSILON = numeric_limits<double>::epsilon();

Hit Plane::intersect(Ray const &ray) {
  /* ******************************************************
   * Intersection Calculation
   * 
   * Given: ray, position, normal
   * Sought: intersect? if true then *t
   * 
   * Ray's Origin (ray.O), Ray's Direction (ray.D),
   * point on plane (position), plane's normal (N)
   * 
   * Finding an intersection is as simple as checking the
   * normals. The only thing to be careful of is dividing
   * by zero. So we first check that.
   * 
   * ****************************************************/
    
  // Detect ray intersection via normal directions */
  double denominator = N.dot(ray.D);
  double t;

  // If the ray is parallel, don't divide by zero
  if (abs(denominator) < EPSILON) {
    return Hit::NO_HIT();
  }

  double numerator = (point - ray.O).dot(N);
  t = numerator / denominator;
  if (t <= EPSILON) { // Plane is behind the origin of the ray
    return Hit::NO_HIT();
  }

  // The normal of the plane is always the normal of the plane
  return Hit(t, N);
}

Plane::Plane(Point const &point, Vector const N) : point(point), N(N) {}
