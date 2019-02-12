#include "mainview.h"

#include "math.h"
#include "vertex.h"

#include <QDateTime>
#include <vector>

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
  debugLogger->stopLogging();

  qDebug() << "MainView destructor";

  // Free VBOs
  glDeleteBuffers(1, &VBO_Cube);
  glDeleteBuffers(1, &VBO_Pyramid);

  // Free VAOs
  glDeleteVertexArrays(1, &VAO_Cube);
  glDeleteVertexArrays(1, &VAO_Pyramid);
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

  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);

  // Enable backface culling
  glEnable(GL_CULL_FACE);

  // Default is GL_LESS
  glDepthFunc(GL_EQUAL);

  // Set the color of the screen to be black on clear (new frame)
  glClearColor(0.2f, 0.5f, 0.7f, 1.0f);

  createShaderProgram();

  // Load the two shapes
  loadCube();
  loadPyramid();

  // Set the initial position
  setInitialTranslation();
  setProjection();
}

void MainView::createShaderProgram() {
  // Create shader program
  shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                        ":/shaders/vertshader.glsl");
  shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                        ":/shaders/fragshader.glsl");

  shaderProgram.link();

  // Fetch location of modelTransform and modelProjection uniform
  uniformTransform = shaderProgram.uniformLocation("modelTransform");
  uniformProjection = shaderProgram.uniformLocation("modelProjection");
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shaderProgram.bind();

  // Set the projection path for the data
  glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
                     projectionTransform.data());

  // Set the transform location for the cube
  glUniformMatrix4fv(uniformTransform, 1, GL_FALSE, cubeTransform.data());
  glBindVertexArray(VAO_Cube);
  glDrawArrays(GL_TRIANGLES, 0, numberOfVerticesCube);

  // Set the transform location for the pyramid
  glUniformMatrix4fv(uniformTransform, 1, GL_FALSE, pyramidTransform.data());
  glBindVertexArray(VAO_Pyramid);
  glDrawArrays(GL_TRIANGLES, 0, numberOfVerticesPyramid);

  shaderProgram.release();
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
  // TODO: Update projection to fit the new aspect ratio
  Q_UNUSED(newWidth)
  Q_UNUSED(newHeight)

  setProjection();
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
  rotationFactor = {static_cast<float>(rotateX), static_cast<float>(rotateY),
                    static_cast<float>(rotateZ)};
  setInitialTranslation();
  update();
}

void MainView::setScale(int scale) {
  scaleFactor = static_cast<float>(scale) / 100.;
  setInitialTranslation();
  update();
}

void MainView::setShadingMode(ShadingMode shading) {
  qDebug() << "Changed shading to" << shading;
  Q_UNIMPLEMENTED();
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

// Creates and loads a unit cube into the GPU buffer
void MainView::loadCube() {
  // This defines the cube to be rendered
  std::vector<Vertex> cube = {// Bottom-Right
                              {{1, -1, 1}, {0, 0, 0}},
                              {{-1, -1, -1}, {0, 0, 0}},
                              {{1, -1, -1}, {0, 0, 0}},
                              // Top-Right
                              {{-1, 1, -1}, {1, 0, 0}},
                              {{1, 1, 1}, {1, 0, 0}},
                              {{1, 1, -1}, {1, 0, 0}},
                              // Right-Right
                              {{1, 1, -1}, {0, 0, 1}},
                              {{1, -1, 1}, {0, 1, 0}},
                              {{1, -1, -1}, {0, 0, 1}},
                              // Front-Right
                              {{1, 1, 1}, {0, 0, 1}},
                              {{-1, -1, 1}, {0, 1, 0}},
                              {{1, -1, 1}, {0, 0, 1}},
                              // Left-Left
                              {{-1, -1, 1}, {0, 1, 0}},
                              {{-1, 1, -1}, {1, 0, 0}},
                              {{-1, -1, -1}, {1, 0, 0}},
                              // Back-Right
                              {{1, -1, -1}, {0, 0, 0}},
                              {{-1, 1, -1}, {0, 0, 0}},
                              {{1, 1, -1}, {0, 1, 0}},
                              // Bottom-Left
                              {{1, -1, 1}, {0, 0, 0}},
                              {{-1, -1, 1}, {0, 0, 0}},
                              {{-1, -1, -1}, {0, 0, 0}},
                              // Top-Left
                              {{-1, 1, -1}, {0, 1, 1}},
                              {{-1, 1, 1}, {0, 1, 1}},
                              {{1, 1, 1}, {0, 1, 1}},
                              // Right-Left
                              {{1, 1, -1}, {0, 0, 1}},
                              {{1, 1, 1}, {0, 0, 1}},
                              {{1, -1, 1}, {0, 1, 0}},
                              // Front-Left
                              {{1, 1, 1}, {0, 0, 1}},
                              {{-1, 1, 1}, {1, 0, 0}},
                              {{-1, -1, 1}, {0, 1, 0}},
                              // Left-Right
                              {{-1, -1, 1}, {0, 1, 0}},
                              {{-1, 1, 1}, {1, 0, 0}},
                              {{-1, 1, -1}, {1, 0, 0}},
                              // Back-Left
                              {{1, -1, -1}, {0, 0, 0}},
                              {{-1, -1, -1}, {0, 0, 0}},
                              {{-1, 1, -1}, {0, 0, 0}}};

  numberOfVerticesCube = cube.size();

  // Generate the buffer and vertex array, and return the IDs to upload the cube
  // to
  glGenBuffers(1, &VBO_Cube);
  glGenVertexArrays(1, &VAO_Cube);

  // Now send the vertices(cube) to the GPU vind bind
  glBindVertexArray(VAO_Cube);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
  glBufferData(GL_ARRAY_BUFFER, cube.size() * (sizeof(Vertex)), cube.data(),
               GL_STATIC_DRAW);

  // Now inform the GPU what attributes to use for received arrays
  // via the contents of the shader files
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // Finally inform the layout of the data for the attributes
  // With OFFSET equal to the size of the coordinate array
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void *>(sizeof(Vertex::coordinates)));
}

void MainView::loadPyramid() {
  // This defines the pyramid to be rendered
  std::vector<Vertex> pyramid = {
      // Left
      {{0, 1, 0}, {0, 1, 0}},
      {{-1, -1, -1}, {0, 1, 0}},
      {{-1, -1, 1}, {0, 1, 0}},
      // Bottom-Right
      {{1, -1, 1}, {0, 0, 0}},
      {{-1, -1, -1}, {0, 0, 0}},
      {{1, -1, -1}, {0, 0, 0}},
      // Back
      {{0, 1, 0}, {0, 0, 1}},
      {{1, -1, -1}, {0, 0, 1}},
      {{-1, -1, -1}, {0, 0, 1}},
      // Bottom-Left
      {{1, -1, 1}, {0, 0, 0}},
      {{-1, -1, 1}, {0, 0, 0}},
      {{-1, -1, -1}, {0, 0, 0}},
      // Right
      {{0, 1, 0}, {1, 0, 0}},
      {{1, -1, 1}, {0, 0, 1}},
      {{1, -1, -1}, {1, 0, 0}},
      // Front
      {{0, 1, 0}, {1, 0, 0}},
      {{-1, -1, 1}, {0, 1, 0}},
      {{1, -1, 1}, {0, 0, 1}},
  };

  numberOfVerticesPyramid = pyramid.size();

  // Generate the buffer and vertex array, and return the IDs to upload the cube
  // to
  glGenBuffers(1, &VBO_Pyramid);
  glGenVertexArrays(1, &VAO_Pyramid);

  // Now send the vertices(cube) to the GPU vind bind
  glBindVertexArray(VAO_Pyramid);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Pyramid);
  glBufferData(GL_ARRAY_BUFFER, pyramid.size() * (sizeof(Vertex)),
               pyramid.data(), GL_STATIC_DRAW);

  // Now inform the GPU what attributes to use for received arrays
  // via the contents of the shader files
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // Finally inform the layout of the data for the attributes
  // With OFFSET equal to the size of the coordinate array
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void *>(sizeof(Vertex::coordinates)));
}

               GL_STATIC_DRAW);

  // Now inform the GPU what attributes to use for received arrays
  // via the contents of the shader files
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // Finally inform the layout of the data for the attributes
  // With OFFSET equal to the size of the coordinate array
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void *>(sizeof(Vertex::coordinates)));
}

void MainView::setInitialTranslation() {
  // Initialize as identify matrix
  cubeTransform.setToIdentity();
  pyramidTransform.setToIdentity();

  // Translate to beginig position (2,0,-6) and (-2,0,-6)
  cubeTransform.translate(2, 0, -6);
  pyramidTransform.translate(-2, 0, -6);

  // Apply the scale factor 1.0 or provided by the GUI
  cubeTransform.scale(scaleFactor);
  pyramidTransform.scale(scaleFactor);
  // Apply the rotations
  cubeTransform.rotate(QQuaternion::fromEulerAngles(rotationFactor));
  pyramidTransform.rotate(QQuaternion::fromEulerAngles(rotationFactor));
}

void MainView::setProjection() {
  // This sets the POV and position of camera initial camera
  float aspectRatio =
      static_cast<float>(width()) / static_cast<float>(height());

  // Set perspective
  projectionTransform.setToIdentity();
  projectionTransform.perspective(60.0, aspectRatio, 0, 1);
}
