#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector3D>
#include <memory>

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
  Q_OBJECT

  QOpenGLDebugLogger *debugLogger;
  QTimer timer; // timer used for animation

  QOpenGLShaderProgram shaderProgram;

  // Store the ID of the uniforms
  GLint uniformTransform;
  GLint uniformProjection;

  // Store the ID of each VAO and VBO
  GLuint VBO_Cube;
  GLuint VAO_Cube;
  GLuint VBO_Pyramid;
  GLuint VAO_Pyramid;
  GLuint VBO_Sphere;
  GLuint VAO_Sphere;

  // Store the number of vertices for each shape
<<<<<<< HEAD
  int numberOfVerticesCube;
  int numberOfVerticesPyramid;
  int numberOfVerticesSphere;

  float scaleFactor = 1.0f;
  QVector3D rotationFactor = {0.0, 0.0, 0.0};
=======
  size_t numberOfVerticesCube;
  size_t numberOfVerticesPyramid;
  size_t numberOfVerticesSphere;

  float scaleFactor = 1.0f;
  QQuaternion rotationFactor = QQuaternion::fromEulerAngles({0.0, 0.0, 0.0});
>>>>>>> 103234505f7d53f83926777824b721fa562bcc62

  // Store a transform matrix for each object, including projection
  QMatrix4x4 cubeTransform;
  QMatrix4x4 pyramidTransform;
  QMatrix4x4 sphereTransform;
  QMatrix4x4 projectionTransform;

public:
  enum ShadingMode : GLuint { PHONG = 0, NORMAL, GOURAUD };

  MainView(QWidget *parent = 0);
  ~MainView();

  // Functions for widget input events
  void setRotation(int rotateX, int rotateY, int rotateZ);
  void setScale(int scale);
  void setShadingMode(ShadingMode shading);

protected:
  void initializeGL();
  void resizeGL(int newWidth, int newHeight);
  void paintGL();

  // Functions for keyboard input events
  void keyPressEvent(QKeyEvent *ev);
  void keyReleaseEvent(QKeyEvent *ev);

  // Function for mouse input events
  void mouseDoubleClickEvent(QMouseEvent *ev);
  void mouseMoveEvent(QMouseEvent *ev);
  void mousePressEvent(QMouseEvent *ev);
  void mouseReleaseEvent(QMouseEvent *ev);
  void wheelEvent(QWheelEvent *ev);

private slots:
  void onMessageLogged(QOpenGLDebugMessage Message);

private:
  void createShaderProgram();
  void loadCube();
  void loadPyramid();
  void loadSphere();
  void setInitialTranslation();
  void setProjection();
};

#endif // MAINVIEW_H
