#include "mesh.h"
#include "triangle.h"

#include <cmath>

using namespace std;

Hit Mesh::intersect(Ray const &ray) {

  Hit min_hit(numeric_limits<double>::infinity(), Vector());
  bool hit = false;
  auto vertices = model.vertex_data();

  // Iterate through each triangle
  // Looking for the closest hit
  for (size_t i = 0; i < model.numTriangles(); ++i) {
    Point vertex1 =
        Point(vertices[i].x, vertices[i].y, vertices[i].z) + translation;
    Point vertex2 =
        Point(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z) +
        translation;
    Point vertex3 = Point(vertices[i + 2].x, vertices[i + 2].y,
                          vertices[i + 2].z) = translation;
    Triangle triangle(vertex1, vertex2, vertex3);

    Hit intersection(triangle.intersect(ray));
    if (intersection.t < min_hit.t) {
      min_hit = intersection;
      hit = true;
    }
  }

  return hit ? min_hit : Hit::NO_HIT();
}

Mesh::Mesh(string const &filename, Vector const &translation)
    : filename(filename), model(OBJLoader(filename)), translation(translation) {
}
