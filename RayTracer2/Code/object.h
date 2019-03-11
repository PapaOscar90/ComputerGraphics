#ifndef OBJECT_H_
#define OBJECT_H_

#include "material.h"

// not really needed here, but deriving classes may need them
#include "hit.h"
#include "ray.h"
#include "triple.h"

#include <memory>
class Object;
typedef std::shared_ptr<Object> ObjectPtr;

typedef struct TextureCoordinates{
  float x;
  float y;
} TextureCoordiantes;

class Object {
public:
  Material material;
  

  virtual ~Object() = default;

  virtual Hit intersect(Ray const &ray) = 0; // must be implemented
                                             // in derived class

  virtual TextureCoordiantes findTextureCoords(Point hit, Object &self);
};

#endif
