#ifndef VERTEX_H
#define VERTEX_H

struct Coordinates {
  float X, Y, Z;
};

// Color is a mixture in order: R,G,B
struct Color {
  float R, G, B;
};

// The vertex contains in order: Coordinates, Color
struct Vertex {
  // The Coordinates in order: X,Y,Z
  Coordinates coordinates;
  Color color;
};

#endif // VERTEX_H
