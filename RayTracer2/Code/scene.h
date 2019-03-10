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

protected:
  // For internal calculation of reflection light amount
  Color getColorAt(Material material, Point hit, Vector N, Vector V,
                   int impactsToUse);
  int isInShadow(Point hit, Vector N, Vector L);
  Color getDiffuseColor(Material material, LightPtr lightPtr, Vector N,
                        Vector L);
  Color getSpecularColor(Material material, LightPtr lightPtr, Vector N,
                         Vector L, Vector V);
  Color getReflectionColor(Material material, Point hit, Vector N, Vector V,
                           int impactsRemaining);

public:
  // trace a ray into the scene and return the color
  Color trace(Ray const &ray, int useReflections);

  // render the scene to the given image
  void render(Image &img, int superSamp, int impactsToUse);

  void addObject(ObjectPtr obj);
  void addLight(Light const &light);
  void setEye(Triple const &position);

  unsigned getNumObject();
  unsigned getNumLights();
};

#endif
