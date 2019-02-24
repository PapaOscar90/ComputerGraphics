#include "plane.h"

#include <cmath>

Hit Plane::intersect(Ray const &ray) {
  /* Detect ray intersection via normal directions */
  float denominator = n.dot(ray.D);
  double t;

  // If the ray is parallel, don't divide by zero
  // also if the ray is in the opposite direction just return
  if(denominator < 0.0000001){
    return Hit::NO_HIT();
  }else{
    Vector temp = position - ray.O;
    t = temp.dot(n) / denominator;
  }

  // The normal of the plane is always the normal of the plane
  return Hit(t, n);
}

Plane::Plane(Point const &pos, Vector const n) : position(pos), n(n){}