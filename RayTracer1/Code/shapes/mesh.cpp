#include "mesh.h"

#include "../objloader.h"
#include <cmath>

using namespace std;

Hit Mesh::intersect(Ray const &ray)
{

  Hit min_hit(numeric_limits<double>::infinity(), Vector());
  bool hit = false;

  // Iterate through each triangle
  // Looking for the closest hit
  for (Triangle triangle : triangles)
  {

    Hit intersection(triangle.intersect(ray));
    if (intersection.t < min_hit.t)
    {
      min_hit = intersection;
      hit = true;
    }
  }

  return hit ? min_hit : Hit::NO_HIT();
}

Mesh::Mesh(string const &filename, Vector const &translation)
    : filename(filename), translation(translation)
{
  OBJLoader model(filename);
  auto vertices = model.vertex_data();
  for (size_t i = 0; i < model.numTriangles(); i++)
  {
    Point vertex1 = Point(vertices[3 * i].x, vertices[3 * i].y, vertices[3 * i].z) +
                    translation;
    Point vertex2 = Point(vertices[3 * i + 1].x, vertices[3 * i + 1].y,
                          vertices[3 * i + 1].z) +
                    translation;
    Point vertex3 = Point(vertices[3 * i + 2].x, vertices[3 * i + 2].y,
                          vertices[3 * i + 2].z) +
                    translation;
    Triangle triangle(vertex1, vertex2, vertex3);
    triangles.push_back(triangle);
  }
}
