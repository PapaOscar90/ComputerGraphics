#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

using namespace std;

Color Scene::trace(Ray const &ray, int impactsRemaining) {
  if(impactsRemaining<1)
    return Color(0.0, 0.0, 0.0);

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

  /****************************************************
   * This is where you should insert the color
   * calculation (Phong model).
   *
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
    
    // Detect if light in lights is line of sight with impact location
    int isInShadow = 0;
    Point shadowOffset = hit + (0.00001*N); // Offset to account for precision
    Ray shadowRay = Ray(shadowOffset, L); // Create ray towards light from impact
    ObjectPtr shadowObject = nullptr;
    Hit min_hit(numeric_limits<double>::infinity(), Vector());

    // For each object, detect if there is a collision
    for (unsigned idx = 0; idx != objects.size(); ++idx) { 
      Hit shadowHit(objects[idx]->intersect(shadowRay));
      if (shadowHit.t > 0) {
        isInShadow = 1; // If there is, set flag
      }
    }

    // If the impact is not in a shadow, add the light to color
    if(!isInShadow){
      // Diffuse term
      float NdotL = NHat.dot(L);
      float intensity = max(min(NdotL, 1.0f), 0.0f);
      color += material.kd * intensity * material.color * (lightPtr->color);

      // Specular term
      Vector R = (2 * (NdotL)*NHat - L).normalized();
      float VdotR = VHat.dot(R);
      intensity = pow(max(min(VdotR, 1.0f), 0.0f), material.n);
      color += material.ks * intensity * (lightPtr->color);
    }
  }

  // Now bounce the ray to the next impact to find the relfection color
  // Get new vector
  float NdotV = N.dot(ray.D.normalized());
  Vector B = (2 * (NdotV)*NHat - (ray.D)).normalized();

  Ray bounceRay(ray.O, B);

  return color + trace(bounceRay, impactsRemaining - 1);
}

void Scene::render(Image &img) {
  unsigned w = img.width();
  unsigned h = img.height();

#pragma omp parallel for
  for (unsigned y = 0; y < h; ++y) {
    for (unsigned x = 0; x < w; ++x) {
      Point pixel(x + 0.5, h - 1 - y + 0.5, 0);
      Ray ray(eye, (pixel - eye).normalized());
      Color col = trace(ray, 2);
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
