#include "triangle.h"

#include <cmath>
#include <limits>

using namespace std;

const double EPSILON = numeric_limits<double>::epsilon();

Hit Triangle::intersect(Ray const &ray) {
  /* *********************************************************************
   * RT1.1: INTERSECTION CALCULATION
   *
   * Given: ray, vertices[]     * Sought: intersects? if true: *t
   *
   * Insert calculation of ray/triangle intersection here.
   *
   * You have the triangle'tvec vertices (vertices) and  the
   * ray'tvec origin (ray.O) and direction (ray.D).
   *
   * Computation for Intersection:
   * If the ray does not intersect the triangle, return NoHit.
   * Otherwise, return Hit and place the distance of the
   * intersection point from the ray origin in *t (see example).
   *
   * If an intersection occurs and the distance along the ray if it does occur
   * is computer using the Möller–Trumbore intersection algorithm.
   * See http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
   *
   ***************************************************************************/

  Vector edge1(vertices[1] - vertices[0]);
  Vector edge2(vertices[2] - vertices[0]);
  Vector pvec = ray.D.cross(edge2);
  double det = edge1.dot(pvec);

  if (det > -EPSILON && det < EPSILON)
    return Hit::NO_HIT(); // The ray is parallel to the triangle.

  double invDet = 1.0 / det;
  Vector tvec = ray.O - vertices[0];
  double u = invDet * (tvec.dot(pvec));
  if (u < 0.0 || u > 1.0)
    return Hit::NO_HIT();

  Vector qvec = tvec.cross(edge1);
  double v = invDet * (ray.D.dot(qvec));
  if (v < 0.0 || u + v > 1.0)
    return Hit::NO_HIT();

  double t = invDet * (edge2.dot(qvec));
  if (t <= EPSILON) {
    return Hit::NO_HIT();
  }
  /**************************************************************************
   * RT1.2: NORMAL CALCULATION
   *
   * Given: t, C, r
   * Sought: N
   *
   * As a triangle s effective a bounded planar surface, the surface normal is
   *simple the normal off of the plane. This may be found simply by performing
   *the cross product of the two edges of the triangle.
   *
   *
   **************************************************************************/

  Vector N = edge2.cross(edge1);
  if (N.dot(ray.D) > 0)
    N = -N;

  return Hit(t, N);
}

Triangle::Triangle(Point const &vertex1, Point const &vertex2,
                   Point const &vertex3)
    : vertices({vertex1, vertex2, vertex3}) {}
