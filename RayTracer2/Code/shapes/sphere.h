#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"

class Sphere : public Object {
public:
  Sphere(Point const &pos, double radius);

  virtual Hit intersect(Ray const &ray);
  virtual TextureCoordiantes findTextureCoords(Point &hit, Object &self);

  Point const position;
  double const r;
  TextureCoordiantes textureCoordiantes;
};

#endif
