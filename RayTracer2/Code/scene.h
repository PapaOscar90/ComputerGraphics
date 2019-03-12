#ifndef SCENE_H_
#define SCENE_H_

#include "light.h"
#include "object.h"
#include "triple.h"

#include <vector>

// Forward declerations
class Ray;
class Image;

class Scene {
  std::vector<ObjectPtr> objects;
  std::vector<LightPtr> lights; // no ptr needed, but kept for consistency
  Point eye;

  // Additional configuration
  bool renderShadows = false;
  double shadowBias = 0.00001;
  double reflectionBias = 0.00000000001;
  unsigned int ssFactor = 1;
  unsigned int recursionDepth = 1;

public:
  // trace a ray into the scene and return the color
  Color trace(Ray const &ray, int depth);

  // render the scene to the given image
  void render(Image &img);

  void addObject(ObjectPtr obj);
  void addLight(Light const &light);
  void setEye(Triple const &position);
  void shouldRenderShadows(bool shadows);
  void setSuperSamplingFactor(unsigned int factor);
  void setRecursionFactor(unsigned int depth);

  unsigned getNumObject();
  unsigned getNumLights();

private:
  Color getColor(Ray const &ray, ObjectPtr obj, Hit const &hit, int depth);
  bool inShadow(Point hit, Vector N, Vector L);
};

#endif
