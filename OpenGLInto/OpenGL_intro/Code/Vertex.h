#ifndef TRIANGLE_H
#define TRIANGLE_H

struct Coordinates{
    float X,Y;
};

struct Color{
    float R,G,B;
};

// A Vertex has a coordinate X,Y and color R,G,B
struct Vertex{
    Coordinates coordinates;
    Color color;
};

#endif // TRIANGLE_H
