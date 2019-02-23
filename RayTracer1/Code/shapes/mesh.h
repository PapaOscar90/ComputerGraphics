#ifndef MESH_H_
#define MESH_H_

#include "../object.h"
#include "../objloader.h"
#include <string>

class Mesh : public Object {
public:
  Mesh(std::string const &filename, Vector const &translation);

  virtual Hit intersect(Ray const &ray);

private:
  std::string const &filename;
  OBJLoader const model;
  Vector const translation;
};

#endif
