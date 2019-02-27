#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertNormal;
in vec3 transformedVertCoordinates;
in vec3 transformedLightCoordinates;

// Specify the Uniforms of the fragment shaders
// uniform vec3 lightPosition; // for example
uniform vec4 material;
uniform vec3 lightColor;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main() {
  vec3 materialColor = vec3(1, 1, 1);

  // AMBIENT TERM
  vec3 color = material.x * materialColor;

  vec3 L = normalize(transformedLightCoordinates - transformedVertCoordinates);
  vec3 normal = normalize(vertNormal);

  // DIFFUSE TERM
  float NDotL = dot(normal, L);
  float intensity = max(min(NDotL, 1.0), 0.0);
  color += material.y * intensity * lightColor * materialColor;

  //  SPECULAR TERM
  vec3 R = 2.0 * NDotL * normal - L;
  vec3 V = -1 * normalize(transformedVertCoordinates);
  float VDotR = dot(R, V);
  intensity = pow(max(min(VDotR, 1.0), 0.0), material.w);
  color += material.z * intensity * lightColor;

  fColor = vec4(color, 1.0);
}
