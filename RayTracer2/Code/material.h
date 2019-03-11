#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "texture.h"
#include "triple.h"

#include "mapbox/variant.hpp"
#include <iostream>

class Material {
  using Surface = mapbox::util::variant<Color, Texture>;

public:
  Surface surface;
  double ka; // ambient intensity
  double kd; // diffuse intensity
  double ks; // specular intensity
  double n;  // exponent for specular highlight size

  Material() = default;

  Material(Color const &color, double ka, double kd, double ks, double n)
      : surface(color), ka(ka), kd(kd), ks(ks), n(n) {
    std::cout << "Setting color\n";
  }

  Material(Texture const &texture, double ka, double kd, double ks, double n)
      : surface(texture), ka(ka), kd(kd), ks(ks), n(n) {
    std::cout << "Setting texture\n";
  }
};

#endif
