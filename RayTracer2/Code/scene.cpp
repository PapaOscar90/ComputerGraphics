#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

using namespace std;

Color Scene::trace(Ray const &ray, int depth) {
  // If we have reached the final impact already, return black
  if (depth < 1) {
    return Color(0.0, 0.0, 0.0);
  }

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

  // Get color of impact
  return getColor(ray, obj, min_hit, depth);
}

void Scene::render(Image &img) {
  unsigned w = img.width();
  unsigned h = img.height();

  int factor = ssFactor;
  double subPixelSize = 1.0 / (2 * factor);

  #pragma omp parallel for
  for (unsigned y = 0; y < h; ++y) {
    for (unsigned x = 0; x < w; ++x) {

      // Apply super sampling
      // Average the color over the samples
      Color col(0., 0., 0.);
      for (int i = -factor / 2; i <= factor / 2; ++i) {
        for (int j = -factor / 2; j <= factor / 2; ++j) {
          // If there is more than one pixel, the 0 crossings should be skipped
          if (factor != 1 && (i == 0 || j == 0))
            continue;

          // Determine coordinates of sample
          Point pixel(x + 0.5 + (subPixelSize * i),
                      h - 1 - y + 0.5 + (subPixelSize * j), 0);
          Ray ray(eye, (pixel - eye).normalized());
          // Collect color of samples
          col += trace(ray, recursionDepth);
        }
      }
      col /= ssFactor * ssFactor; // Average the colors over the samples

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

void Scene::shouldRenderShadows(bool shadows) { renderShadows = shadows; }

// Checks if the object is in the shadow of another object
bool Scene::inShadow(Point hit, Vector N, Vector L) {
  Point shadowOrigin = hit + (shadowBias * N);
  Ray shadowRay(shadowOrigin, L);

  ObjectPtr shadowObject = nullptr;

  // Check if the shadow ray collides with any object going towards the light
  for (unsigned idx = 0; idx != objects.size(); ++idx) {
    Hit shadowHit(objects[idx]->intersect(shadowRay));
    if (shadowHit.t > 0) { // There is a collision
      return true;
    }
  }
  return false;
}

// Returns a color at an intersection with an object
Color Scene::getColor(Ray const &ray, ObjectPtr obj, Hit const &intersection,
                      int depth) {
  Material material = obj->material; // the hit objects material

  Point hit = ray.at(intersection.t); // the hit point
  Vector N = intersection.N;          // the normal at hit point
  Vector V = -ray.D;                  // the view vector

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

  // Return either the color or texture depending on material type
  Color materialColor = material.surface.match(
      [=](Color materialColor) { return materialColor; },
      [=](Texture &materialTexture) {
        auto coordinates = obj->textureCoordinates(hit);

        return materialTexture.colorAt(coordinates.u, coordinates.v);
      });

  // Add the ambient component
  Color color = material.ka * AMBIENT_LIGHT_INTENSITY * materialColor;

  // For each light
  for (auto lightPtr : lights) {
    // Create vector to light
    Vector L = (lightPtr->position - hit).normalized();

    // If the impact is in shadow, then the light does not contribute.
    if (renderShadows && inShadow(hit, N, L))
      continue;

    // Diffuse term
    float NdotL = NHat.dot(L);
    float intensity = max(min(NdotL, 1.0f), 0.0f);
    color += material.kd * intensity * materialColor * (lightPtr->color);

    // Specular term
    Vector R = (2 * (NdotL)*NHat - L).normalized();
    float VdotR = VHat.dot(R);
    intensity = pow(max(min(VdotR, 1.0f), 0.0f), material.n);
    color += material.ks * intensity * (lightPtr->color);
  }

  // Create the reflection ray for recursive reflection
  Vector reflectionDirection = ray.D - N * 2.0 * ray.D.dot(N);
  Point reflectionOrigin = hit  + (N*0.00000000001); // Bias
  Ray reflectionRay(reflectionOrigin,reflectionDirection);

  // Return the light at the current hit, plus the light being reflected onto this hit
  color += material.ks * trace(reflectionRay, depth - 1);

  return color;
}

unsigned Scene::getNumObject() { return objects.size(); }

void Scene::setSuperSamplingFactor(unsigned int factor) { ssFactor = factor; }

void Scene::setRecursionFactor(unsigned int depth) { recursionDepth = depth; }

unsigned Scene::getNumLights() { return lights.size(); }
