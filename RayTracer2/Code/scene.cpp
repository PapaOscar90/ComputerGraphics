#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

using namespace std;

 const float AMBIENT_LIGHT_INTENSITY = 1.0;

Color Scene::trace(Ray const &ray, int impactsToUse) {
  // Find hit object and distance
  Hit min_hit(numeric_limits<double>::infinity(), Vector());
  ObjectPtr obj = nullptr;

  // Check for a collision for every object
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

  Color color = getColorAt(material, hit, N, V, impactsToUse);

  return color;
}

Color Scene::getColorAt(Material material, Point hit, Vector N, Vector V, int impactsToUse){
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
   Color color = material.ka * AMBIENT_LIGHT_INTENSITY * material.color;

  // Calculate for every light the light intensity at hit location
  for (auto lightPtr : lights) {
    // Compute the light vector, and normalize
    Vector L = (lightPtr->position - hit).normalized();
    
    // If the impact is not in a shadow, add the different light to color
    if(!isInShadow(hit, N, L)){
      color += getDiffuseColor(material, lightPtr, N, L);
      color += getSpecularColor(material, lightPtr, N, L, V);
      color += getReflectionColor(material, hit, N, V, impactsToUse);
    }
  }

  return color;
}

// Diffuse term, depends on angle of light to the surface normal
Color Scene::getDiffuseColor(Material material, LightPtr lightPtr, Vector N, Vector L){
  float NdotL = N.normalized().dot(L);
  float intensity = max(min(NdotL, 1.0f), 0.0f);
  Color color = material.kd * intensity * material.color * (lightPtr->color);

  return color;
}

// Specular term, depends on angle of light reflection to the eye vector
Color Scene::getSpecularColor(Material material, LightPtr lightPtr, Vector N, Vector L, Vector V){
  float NdotL = N.normalized().dot(L);
  Vector NHat = N.normalized();

  Vector R = (2 * (NdotL)*NHat - L).normalized();
  float VdotR = V.normalized().dot(R);
  float intensity = pow(max(min(VdotR, 1.0f), 0.0f), material.n);
  Color color = material.ks * intensity * (lightPtr->color);

  return color;
}

// Find the color being reflected onto the surface
Color Scene::getReflectionColor(Material material, Point hit, Vector N, Vector V, int impactsRemaining){
  // If we have reached the impact location, return nothing
  if (impactsRemaining ==  0){
    return Color(0.0, 0.0, 0.0);
  }
  // Find if there is an object in the reflection path
  Hit min_hit(numeric_limits<double>::infinity(), Vector());
  ObjectPtr obj = nullptr;

  float NdotV = N.normalized().dot(V);
  Vector NHat = N.normalized();
  
  // Compute the direction of the reflection (B)ounce
  Vector B = (V - 2 * (NdotV)*NHat).normalized();

  // Move the hit outwards a bit to not impact itself
  hit = hit + (0.00001*N);
  // Create a ray from the impact in the direction of the relfection vector
  Ray reflectionRay = Ray(hit, B);

  // Check for a hit
  for (unsigned idx = 0; idx != objects.size(); ++idx) {
    Hit hit(objects[idx]->intersect(reflectionRay));
    if (hit.t < min_hit.t) {
      min_hit = hit;
      obj = objects[idx];
    }
  }

  // No hit? Return background color.
  if (!obj)
    return Color(0.0, 0.0, 0.0);

  material = obj->material; // the hit objects material
  hit = reflectionRay.at(min_hit.t);     // the hit point
  N = min_hit.N;              // the normal at hit point
  V = -reflectionRay.D;                 // the view vector

  // Start tracking the color to be returned for the reflection value
  Color returnColor = Color(0.0, 0.0, 0.0);
  for(auto lightPtr : lights){
    Vector L = (lightPtr->position - hit).normalized();
    
    // If the impact is not in a shadow, add the light to color
    if(!isInShadow(hit, N, L)){
      returnColor += getDiffuseColor(material, lightPtr, N, L);
    }
  }

  return (returnColor + getReflectionColor(material,hit, N, V, (impactsRemaining-1)));
}

int Scene::isInShadow(Point hit, Vector N, Vector L){
  Point shadowOffset = hit + (0.00001*N); // Offset to account for precision
  Ray shadowRay = Ray(shadowOffset, L); // Create ray towards light from impact
  ObjectPtr shadowObject = nullptr;

  // For each object, detect if there is a collision
  for (unsigned idx = 0; idx != objects.size(); ++idx) { 
    Hit shadowHit(objects[idx]->intersect(shadowRay));
    if (shadowHit.t > 0) {
      return true; // If there is, set flag
    }
  }
  return false;
}

void Scene::render(Image &img, int superSamp, int impactsToUse) {
  unsigned w = img.width();
  unsigned h = img.height();
  double subPixelSize = (1.0/superSamp)/2.0;


  #pragma omp parallel for
  for (unsigned y = 0; y < h; ++y) {
    for (unsigned x = 0; x < w; ++x) {
      Color col = Color(0.0, 0.0, 0.0);
      for(int j = -superSamp/2; j <= superSamp/2; j++){
        for( int k = -superSamp/2; k <= superSamp/2; k++){
          
          // If there is more than one pixel, skip the o crossings.
          if(superSamp != 1 && (j == 0 || k == 0)) {
            continue;
          }

          // Add +/- the subpixel*factor to either side of the center
          double originalX = x + 0.5;
          double originalY = h - 1 - y + 0.5;
          double pX = originalX + (subPixelSize*j);
          double pY = originalY + (subPixelSize*k);
          Point pixel(pX, pY, 0);
          Ray ray(eye, (pixel - eye).normalized()); 
          col += trace(ray,impactsToUse);
        }
      }
      col = col / (superSamp*superSamp);
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
