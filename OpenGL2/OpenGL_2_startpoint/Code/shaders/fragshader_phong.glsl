#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertNormal;
in vec3 transformedVertCoordinates;
in vec3 transformedLightCoordinates;
in vec2 textureCoordinates;

// Specify the Uniforms of the fragment shaders
// uniform vec3 lightPosition; // for example
uniform vec4 material;
uniform vec3 lightColor;
uniform sampler2D samplerUniform;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main() {
  // Find the color at specified UV coordinates on the 2D texture
  vec3 materialColor = vec3(texture2D(samplerUniform, textureCoordinates));

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

  // Add alpha
  fColor = vec4(color, 1.0);
}
