#ifndef VERTEX_H
#define VERTEX_H

// The vertex contains in order: Coordinates, Color
struct Vertex{
    // The Coordinates in order: X,Y,Z
    struct Coordinates{
        float X,Y,Z;
    };

    // Color is a mixture in order: R,G,B
    struct Color{
        float R,G,B;
    };
};

#endif // VERTEX_H
