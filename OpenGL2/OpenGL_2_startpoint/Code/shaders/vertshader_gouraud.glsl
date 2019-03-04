#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout(location = 0) in vec3 vertCoordinates_in;
layout(location = 1) in vec3 vertNormal_in;
layout(location = 2) in vec2 textureCoordinates_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;
uniform vec4 material;
uniform vec3 lightPosition;

// Specify the output of the vertex stage
out float ambientTerm, diffuseTerm, specularTerm;
out vec2 textureCoordinates;

void main() {
  // gl_Position is the output (a vec4) of the vertex shader
  gl_Position =
      projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0);

  textureCoordinates = textureCoordinates_in;

  // Use the material's ambient property
  ambientTerm = material.x;

  // Apply the transformation to vertices, and lights. This makes sure
  // everything in the world "transforms" together
  vec3 vertNormal =
      normalTransform * vertNormal_in; // Apply normal transformation to normals

  vec3 transformedLightCoordinates =
      vec3(modelViewTransform *
           vec4(lightPosition, 1.0)); // Get transformed light coordinates
  vec3 transformedVertCoordinates =
      vec3(modelViewTransform *
           vec4(vertCoordinates_in, 1.0)); // Get vertex coordinate

  vec3 L = transformedLightCoordinates - transformedVertCoordinates;
  float distance = length(L); // Light drop off
  L = normalize(L);
  vec3 normal = normalize(vertNormal);

  // Calculate light amount for diffuse
  float NDotL = dot(normal, L);
  float intensity = max(min(NDotL, 1.0), 0.0);
  diffuseTerm = material.y * intensity;

  // Calculate light amount for specular, with reflection
  vec3 R = 2.0 * NDotL * normal - L;
  vec3 V = -1 * normalize(transformedVertCoordinates);
  float VDotR = dot(R, V);
  intensity = pow(max(min(VDotR, 1.0), 0.0), material.w);
  specularTerm = material.z * intensity;
}
