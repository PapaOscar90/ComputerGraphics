#ifndef OBJECT_H_
#define OBJECT_H_

#include "material.h"

// not really needed here, but deriving classes may need them
#include "hit.h"
#include "ray.h"
#include "texture.h"
#include "triple.h"

#include <cmath>
#include <iostream>
#include <memory>
class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Object {
public:
  Material material;
  Vector axis;
  double angle = 0;

  virtual ~Object() = default;

  virtual Hit intersect(Ray const &ray) = 0; // must be implemented
                                             // in derived class
  virtual TextureCoordinates textureCoordinates(Point const &point) = 0;

  // Set the axis of rotation, and the angle from vector
  void setRotation(Vector const &vector, double const angle) {
    axis = vector.normalized();
    this->angle = angle * M_PI / 180; // convert between degrees and radians
  }
};

#endif
