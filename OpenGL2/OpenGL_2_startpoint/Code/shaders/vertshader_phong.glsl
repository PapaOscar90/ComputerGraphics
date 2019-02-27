#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout(location = 0) in vec3 vertCoordinates_in;
layout(location = 1) in vec3 vertNormal_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;
uniform vec3 lightPosition;

// Specify the output of the vertex stage
out vec3 vertNormal;
out vec3 transformedVertCoordinates;
out vec3 transformedLightCoordinates;

void main() {
  // gl_Position is the output (a vec4) of the vertex shader
  gl_Position =
      projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0);
  vertNormal =
      normalTransform * vertNormal_in; // Apply normal transformation to normals
  transformedLightCoordinates =
      vec3(modelViewTransform *
           vec4(lightPosition, 1.0)); // Get transformed light coordinates
  transformedVertCoordinates =
      vec3(modelViewTransform *
           vec4(vertCoordinates_in, 1.0)); // Get transformed vertex coordinates
}
