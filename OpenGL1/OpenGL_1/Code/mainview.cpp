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
  glDeleteBuffers(1, &VBO_Sphere);

  // Free VAOs
  glDeleteVertexArrays(1, &VAO_Cube);
  glDeleteVertexArrays(1, &VAO_Pyramid);
  glDeleteVertexArrays(1, &VAO_Sphere);
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
  glDepthFunc(GL_LEQUAL);

  // Set the color of the background
  glClearColor(0.f, 1.f, 0.f, 1.f);

  // Initialize the shaders in the `shaders` subdirectory
  createShaderProgram();

  // Load the shapes into the OpenGL buffers.
  loadShapes();

  // Set the initial positions of shapes and set perspective
  setInitialTranslation();
  setProjection();
}

/**
 * @brief MainView::createShaderProgram
 *
 * Called upon shader program creation
 * Created and links a shader program with attached
 * shader files vertshader and fragshader
 */
void MainView::createShaderProgram() {
  // Create shader program from contents of source files
  shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                        ":/shaders/vertshader.glsl");
  shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                        ":/shaders/fragshader.glsl");

  shaderProgram.link();

  // Fetch location of modelTransform and modelProjection uniform
  // store ID for later
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

  // Set the transform location for the sphere
  glUniformMatrix4fv(uniformTransform, 1, GL_FALSE, sphereTransform.data());
  glBindVertexArray(VAO_Sphere);
  glDrawArrays(GL_TRIANGLES, 0, numberOfVerticesSphere);

  // Set the transform location using the ID for the cube, for the shader
  // computation Then set the Cube for render, and render
  glUniformMatrix4fv(uniformTransform, 1, GL_FALSE, cubeTransform.data());
  glBindVertexArray(VAO_Cube);
  glDrawArrays(GL_TRIANGLES, 0, numberOfVerticesCube);

  // Set the transform location for the pyramid, for the shader computation
  // Then set the pyramid for render and render
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
  // Simply call the setProjection function to automatically
  // fetch and set the projection aspect ratio to new size
  Q_UNUSED(newWidth);
  Q_UNUSED(newHeight);
  setProjection();
}

// --- Public interface
//
/*
 * @brief MainView::setRotation
 *
 * Takes the values from the GUI and updates the rotationFactor vector
 * Then it calls the set...Translation function to update the objects with
 * update
 */
void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
  // Create the quarternion storing the rotation information
  rotationFactor = QQuaternion::fromEulerAngles({static_cast<float>(rotateX),
                                                 static_cast<float>(rotateY),
                                                 static_cast<float>(rotateZ)});
  setInitialTranslation();
  update();
}

/*
 * @brief MainView::setScale
 *
 * This is the same as rotation, but it changes the scaleFactor multiplyer
 *
 * @param scale
 */
void MainView::setScale(int scale) {
  scaleFactor = static_cast<float>(scale) / 100.f;
  setInitialTranslation();
  update();
}

// Not part of current lab
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

/*
 * @brief MainView::loadShapes
 *
 * Load the shapes in the GPU buffer
 */
void MainView::loadShapes() {
//  loadCube();
//  loadPyramid();
  loadSphere();
}

/*
 * @brief MainView::loadCube
 *
 * Creates and loads a unit cube into the GPU buffer
 */
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

  // Generate the vertex buffer object and vertex array object, and return the
  // IDs to upload the cube to.
  glGenBuffers(1, &VBO_Cube);
  glGenVertexArrays(1, &VAO_Cube);

  // Now send the vertices(cube) to the GPU buffer.
  glBindVertexArray(VAO_Cube);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
  glBufferData(GL_ARRAY_BUFFER, cube.size() * (sizeof(Vertex)), cube.data(),
               GL_STATIC_DRAW);

  // Now inform the GPU what attributes to use for received arrays
  // via the contents of the shader files.
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // Finally inform the GPU of the layout of the data for the attributes.
  //   To get the `coordinates` we need an offset of 0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void *>(0));
  //   To get the `color` we need an an offset the size of the coordinates.
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void *>(sizeof(Vertex::coordinates)));
}

/*
 * @brief MainView::loadPyramid
 *
 * Creates and loads a pyramid into the GPU buffer.
 */
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

  // Generate the vertex buffer object and vertex array object, and return the
  // IDs to upload the pyramid to.
  glGenBuffers(1, &VBO_Pyramid);
  glGenVertexArrays(1, &VAO_Pyramid);

  // Now send the vertices(pyramid) to the GPU buffer.
  glBindVertexArray(VAO_Pyramid);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Pyramid);
  glBufferData(GL_ARRAY_BUFFER, pyramid.size() * (sizeof(Vertex)),
               pyramid.data(), GL_STATIC_DRAW);

  // Now inform the GPU what attributes to use for received arrays
  // via the contents of the shader files.
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // Finally inform the GPU of the layout of the data for the attributes.
  //   To get the `coordinates` we need an offset of 0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void *>(0));
  //   To get the `color` we need an an offset the size of the coordinates.
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void *>(sizeof(Vertex::coordinates)));
}

/*
 * @brief MainView::loadSphere
 *
 * Loads a sphere from an obj file and then into the GPU buffer.
 */
void MainView::loadSphere() {
  // Load the provided sphere from an obj file.
  Model model(":/models/cat.obj");
  model.unitize();

  // Prepare the sphere to be loaded into the GPU buffer
  std::vector<Vertex> sphere;

  // Iterate through each vertex in the model and convert that vertex to our
  // `Vertex` class.
  for (auto node : model.getVertices()) {
    // Get the coordinates and set a random color, capped to a max of 1.0.
    Vertex vertex = {{node.x(), node.y(), node.z()},
                     {static_cast<float>(rand()) / RAND_MAX,
                      static_cast<float>(rand()) / RAND_MAX,
                      static_cast<float>(rand()) / RAND_MAX}};
    // Insert the vertex.
    sphere.push_back(vertex);
  }

  numberOfVerticesSphere = sphere.size();

  // Now inform the GPU what attributes to use for received arrays
  // via the contents of the shader files.
  glGenBuffers(1, &VBO_Sphere);
  glGenVertexArrays(1, &VAO_Sphere);

  // Now send the vertices(pyramid) to the GPU buffer.
  glBindVertexArray(VAO_Sphere);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Sphere);
  glBufferData(GL_ARRAY_BUFFER, sphere.size() * (sizeof(Vertex)), sphere.data(),
               GL_STATIC_DRAW);

  // Now inform the GPU what attributes to use for received arrays
  // via the contents of the shader files
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // Finally inform the GPU of the layout of the data for the attributes.
  //   To get the `coordinates` we need an offset of 0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void *>(0));
  //   To get the `color` we need an an offset the size of the coordinates.
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void *>(sizeof(Vertex::coordinates)));
}

/*
 * @brief MainView::setInitialTranslation
 *
 * This function applies transformations to the matrices that transform the
 * objects being rendered. In other words, this sets the objects position
 * (`translate`), size (`scale`), and orientation (`rotate`).
 */
void MainView::setInitialTranslation() {
  // Initialize transformations to the identity matrix
  cubeTransform.setToIdentity();
  pyramidTransform.setToIdentity();
  sphereTransform.setToIdentity();

  // Translate to initial positions provided in specification:
  //   Cube: (2, 0, -6)
  //   Pyramid: (-2, 0, -6)
  //   Sphere: (0, 0, -10)
  cubeTransform.translate(2, 0, -6);
  pyramidTransform.translate(-2, 0, -6);
  sphereTransform.translate(0, 0, -10);

  // Translate from any key press
  cubeTransform.translate(translationFactor);
  pyramidTransform.translate(translationFactor);
  sphereTransform.translate(translationFactor);

  // Apply the scale factor 1.0 or provided by the GUI
  cubeTransform.scale(scaleFactor);
  pyramidTransform.scale(scaleFactor);
  sphereTransform.scale(scaleFactor); // Scaled down by 0.04 to fit

  // Apply the rotations
  cubeTransform.rotate(rotationFactor);
  pyramidTransform.rotate(rotationFactor);
  sphereTransform.rotate(rotationFactor);
}

/*
 * @brief MainView::setProjection
 *
 * This will set the aspect ratio and the perspective matrix
 * that will be applied to the objects.
 */
void MainView::setProjection() {
  // This sets the POV and position of the initial camera
  float aspectRatio =
      static_cast<float>(width()) / static_cast<float>(height());

  // Set the projection to the identity matrix
  projectionTransform.setToIdentity();
  // Set perspective
  projectionTransform.perspective(60.0, aspectRatio, 0, 1);
}
