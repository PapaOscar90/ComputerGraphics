#ifndef MESH_H_
#define MESH_H_

#include "../object.h"
#include "triangle.h"
#include <string>
#include <vector>

class Mesh : public Object {
public:
  Mesh(std::string const &filename, Vector const &translation);

  virtual Hit intersect(Ray const &ray);

private:
  std::string const &filename;
  std::vector<Triangle> triangles;
  Vector const translation;
};

#endif
