#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>

struct Vertex
{
    QVector3D coordinates;
    QVector3D normal;

    Vertex(QVector3D coordinates, QVector3D normal)
        :
          coordinates(coordinates),
          normal(normal)
    {    }
};


#endif // VERTEX_H
