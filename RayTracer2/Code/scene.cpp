#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

using namespace std;

Color Scene::trace(Ray const &ray) {
  // Find hit object and distance
  Hit min_hit(numeric_limits<double>::infinity(), Vector());
  ObjectPtr obj = nullptr;
  for (unsigned idx = 0; idx != objects.size(); ++idx) {
    Hit hit(objects[idx]->intersect(ray));
    if (hit.t < min_hit.t) {
      min_hit = hit;
      obj = objects[idx];
    }
  }

  // No hit? Return background color.
  if (!obj)
    return Color(0.0, 0.0, 0.0);

  Material material = obj->material; // the hit objects material
  Point hit = ray.at(min_hit.t);     // the hit point
  Vector N = min_hit.N;              // the normal at hit point
  Vector V = -ray.D;                 // the view vector

  Color color = getColorAt(material, hit, N, V);

  return color;
}

Color Scene::getColorAt(Material material, Point hit, Vector N, Vector V){
  /****************************************************
   * Given: material, hit, N, V, lights[]
   * Sought: color
   *
   * Hints: (see triple.h)
   *        Triple.dot(Vector) dot product
   *        Vector + Vector    vector sum
   *        Vector - Vector    vector difference
   *        Point - Point      yields vector
   *        Vector.normalize() normalizes vector, returns length
   *        double * Color     scales each color component (r,g,b)
   *        Color * Color      dito
   *        pow(a,b)           a to the power of b
   ****************************************************/

  Vector NHat = N.normalized(); // Normalized N
  Vector VHat = V.normalized(); // Normalized V

  // We add an extra scale factor for us to universally adjust if needed
  const float AMBIENT_LIGHT_INTENSITY = 1.0;

  Color color = material.ka * AMBIENT_LIGHT_INTENSITY * material.color;

  // Calculate for every light the light intensity at hit location
  for (auto lightPtr : lights) {
    Vector L = (lightPtr->position - hit).normalized();
    
    // If the impact is not in a shadow, add the light to color
    if( isInShadow(hit, N, L) == 0){
      color += getDiffuseColor(material, lightPtr, N, L);
      color += getSpecularColor(material, lightPtr, N, L, V);
      color += getReflectionColor(hit, N, V, 1);
    }
  }

  return color;
}

Color Scene::getDiffuseColor(Material material, LightPtr lightPtr, Vector N, Vector L){
    // Diffuse term
  float NdotL = N.normalized().dot(L);
  float intensity = max(min(NdotL, 1.0f), 0.0f);
  Color color = material.kd * intensity * material.color * (lightPtr->color);

  return color;
}

Color Scene::getSpecularColor(Material material, LightPtr lightPtr, Vector N, Vector L, Vector V){
  // Specular term
  float NdotL = N.normalized().dot(L);
  Vector NHat = N.normalized();

  Vector R = (2 * (NdotL)*NHat - L).normalized();
  float VdotR = V.normalized().dot(R);
  float intensity = pow(max(min(VdotR, 1.0f), 0.0f), material.n);
  Color color = material.ks * intensity * (lightPtr->color);

  return color;
}

Color Scene::getReflectionColor(Point hit, Vector N, Vector V, int impactsRemaining){
  if (impactsRemaining <= 1){
    return Color(0.0, 0.0, 0.0);
  }
}

int Scene::isInShadow(Point hit, Vector N, Vector L){
  Point shadowOffset = hit + (0.00001*N); // Offset to account for precision
  Ray shadowRay = Ray(shadowOffset, L); // Create ray towards light from impact
  ObjectPtr shadowObject = nullptr;
  Hit min_hit(numeric_limits<double>::infinity(), Vector());

  // For each object, detect if there is a collision
  for (unsigned idx = 0; idx != objects.size(); ++idx) { 
    Hit shadowHit(objects[idx]->intersect(shadowRay));
    if (shadowHit.t > 0) {
      return true; // If there is, set flag
    }
  }
  return false;
}

void Scene::render(Image &img) {
  unsigned w = img.width();
  unsigned h = img.height();

  #pragma omp parallel for
  for (unsigned y = 0; y < h; ++y) {
    for (unsigned x = 0; x < w; ++x) {
      Point pixel(x + 0.5, h - 1 - y + 0.5, 0);
      Ray ray(eye, (pixel - eye).normalized());
      Color col = trace(ray);
      col.clamp();
      img(x, y) = col;
    }
  }
}

// --- Misc functions ----------------------------------------------------------

void Scene::addObject(ObjectPtr obj) { objects.push_back(obj); }

void Scene::addLight(Light const &light) {
  lights.push_back(LightPtr(new Light(light)));
}

void Scene::setEye(Triple const &position) { eye = position; }

unsigned Scene::getNumObject() { return objects.size(); }

unsigned Scene::getNumLights() { return lights.size(); }
