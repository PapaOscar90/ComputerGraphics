#include "mainview.h"
#include "model.h"
#include "vertex.h"

#include <QDateTime>
#include <math.h>

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << "MainView constructor";

  connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
  makeCurrent();
  debugLogger->stopLogging();
  delete debugLogger;

  qDebug() << "MainView destructor";
  destroyModelBuffers();
  doneCurrent();
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
  qDebug() << ":: Initializing OpenGL";
  initializeOpenGLFunctions();

  debugLogger = new QOpenGLDebugLogger();
  connect(debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
          SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

  if (debugLogger->initialize()) {
    qDebug() << ":: Logging initialized";
    debugLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
    debugLogger->enableMessages();
  }

  QString glVersion;
  glVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDepthFunc(GL_LEQUAL);
  glClearColor(0.0, 1.0, 0.0, 1.0);

  createShaderProgram();
  loadMesh();

  // Initialize transformations
  updateProjectionTransform();
  updateModelTransforms();
}

void MainView::createShaderProgram() {
  // Create shader programs
  // Normal
  normalShaderProgram.addShaderFromSourceFile(
      QOpenGLShader::Vertex, ":/shaders/vertshader_normal.glsl");
  normalShaderProgram.addShaderFromSourceFile(
      QOpenGLShader::Fragment, ":/shaders/fragshader_normal.glsl");
  normalShaderProgram.link();
  // Phong
  phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                             ":/shaders/vertshader_phong.glsl");
  phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                             ":/shaders/fragshader_phong.glsl");
  phongShaderProgram.link();
  // Gourand
  gourandShaderProgram.addShaderFromSourceFile(
      QOpenGLShader::Vertex, ":/shaders/vertshader_gourand.glsl");
  gourandShaderProgram.addShaderFromSourceFile(
      QOpenGLShader::Fragment, ":/shaders/fragshader_gourand.glsl");
  gourandShaderProgram.link();

  // Get the uniforms
  // Normal
  normalUniformModelViewTransform =
      normalShaderProgram.uniformLocation("modelViewTransform");
  normalUniformProjectionTransform =
      normalShaderProgram.uniformLocation("projectionTransform");
  normalUniformNormalTransform =
      normalShaderProgram.uniformLocation("normalTransform");
  // Phong
  phongUniformModelViewTransform =
      phongShaderProgram.uniformLocation("modelViewTransform");
  phongUniformProjectionTransform =
      phongShaderProgram.uniformLocation("projectionTransform");
  phongUniformNormalTransform =
      phongShaderProgram.uniformLocation("normalTransform");
  phongUniformMaterial = phongShaderProgram.uniformLocation("material");
  phongUniformLightPosition = phongShaderProgram.uniformLocation("lightPosition");
  phongUniformLightColor = phongShaderProgram.uniformLocation("lightColor");
  // Gourand
  gourandUniformModelViewTransform =
      gourandShaderProgram.uniformLocation("modelViewTransform");
  gourandUniformProjectionTransform =
      gourandShaderProgram.uniformLocation("projectionTransform");
  gourandUniformNormalTransform =
      gourandShaderProgram.uniformLocation("normalTransform");
  gourandUniformMaterial = gourandShaderProgram.uniformLocation("material");
  gourandUniformLightPosition = gourandShaderProgram.uniformLocation("lightPosition");
  gourandUniformLightColor = gourandShaderProgram.uniformLocation("lightColor");
}

void MainView::loadMesh() {
  Model model(":/models/cat.obj");
  model.unitize();
  QVector<float> meshData = model.getVNInterleaved();
  meshSize = model.getVertices().size();

  // Generate VAO
  glGenVertexArrays(1, &meshVAO);
  glBindVertexArray(meshVAO);

  // Generate VBO
  glGenBuffers(1, &meshVBO);
  glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

  // Write the data to the buffer
  glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float),
               meshData.data(), GL_STATIC_DRAW);

  // Set vertex coordinates to location 0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  // Set normal coordinates to location 1
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
  // Clear the screen before rendering
  glClearColor(0.2f, 0.5f, 0.7f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  switch (activeShader) {
  case NORMAL:
    normalShaderProgram.bind();

    // Set the projection matrix
    glUniformMatrix4fv(normalUniformProjectionTransform, 1, GL_FALSE,
                       projectionTransform.data());
    glUniformMatrix4fv(normalUniformModelViewTransform, 1, GL_FALSE,
                       meshTransform.data());
    glUniformMatrix3fv(normalUniformNormalTransform, 1, GL_FALSE,
                       normalTransform.data());

    glBindVertexArray(meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);

    normalShaderProgram.release();
    break;
  case PHONG:
    phongShaderProgram.bind();

    // Set the projection matrix
    glUniformMatrix4fv(phongUniformProjectionTransform, 1, GL_FALSE,
                       projectionTransform.data());
    glUniformMatrix4fv(phongUniformModelViewTransform, 1, GL_FALSE,
                       meshTransform.data());
    glUniformMatrix3fv(phongUniformNormalTransform, 1, GL_FALSE,
                       normalTransform.data());
    glUniform4fv(phongUniformMaterial, 1, material.data());
    glUniform3fv(phongUniformLightPosition, 1, lightPosition.data());
    glUniform3fv(phongUniformLightColor, 1, lightColor.data());

    glBindVertexArray(meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);

    phongShaderProgram.release();
    break;
  case GOURAUD:
    gourandShaderProgram.bind();

    // Set the projection matrix
    glUniformMatrix4fv(gourandUniformProjectionTransform, 1, GL_FALSE,
                       projectionTransform.data());
    glUniformMatrix4fv(gourandUniformModelViewTransform, 1, GL_FALSE,
                       meshTransform.data());
    glUniformMatrix3fv(gourandUniformNormalTransform, 1, GL_FALSE,
                       normalTransform.data());

    glUniform4fv(gourandUniformMaterial, 1, material.data());
    glUniform3fv(gourandUniformLightPosition, 1, lightPosition.data());
    glUniform3fv(gourandUniformLightColor, 1, lightColor.data());

    glBindVertexArray(meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);

    gourandShaderProgram.release();
    break;
  }
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight) {
  Q_UNUSED(newWidth)
  Q_UNUSED(newHeight)
  updateProjectionTransform();
}

void MainView::updateProjectionTransform() {
  float aspect_ratio =
      static_cast<float>(width()) / static_cast<float>(height());
  projectionTransform.setToIdentity();
  projectionTransform.perspective(60, aspect_ratio, 0.2, 20);
}

void MainView::updateModelTransforms() {
  meshTransform.setToIdentity();
  meshTransform.translate(0, 0, -10);
  meshTransform.scale(scale);
  meshTransform.rotate(QQuaternion::fromEulerAngles(rotation));
  normalTransform = meshTransform.normalMatrix();

  update();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers() {
  glDeleteBuffers(1, &meshVBO);
  glDeleteVertexArrays(1, &meshVAO);
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
  rotation = {static_cast<float>(rotateX), static_cast<float>(rotateY),
              static_cast<float>(rotateZ)};
  updateModelTransforms();
}

void MainView::setScale(int newScale) {
  scale = static_cast<float>(newScale) / 100.f;
  updateModelTransforms();
}

void MainView::setShadingMode(ShadingMode shading) {
  qDebug() << "Changed shading to" << shading;
  activeShader = shading;
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message) {
  qDebug() << " → Log:" << Message;
}
