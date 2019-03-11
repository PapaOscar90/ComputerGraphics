#ifndef OBJECT_H_
#define OBJECT_H_

#include "material.h"

// not really needed here, but deriving classes may need them
#include "hit.h"
#include "ray.h"
#include "texture.h"
#include "triple.h"

#include <memory>
class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Object {
public:
  Material material;
  Vector rotation{0.0, 0.0, 0.0};

  virtual ~Object() = default;

  virtual Hit intersect(Ray const &ray) = 0; // must be implemented
                                             // in derived class
  virtual TextureCoordinates textureCoordinates(Point const &point) = 0;
};

#endif
