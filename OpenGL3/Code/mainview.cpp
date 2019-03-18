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
  debugLogger->stopLogging();

  qDebug() << "MainView destructor";

  // glDeleteTextures(1, &object.myTextureID);

  destroyModelBuffers();
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

  ObjectProperties object;
  loadMesh(":/models/cat.obj", object);
  loadTextures(":/textures/cat_diff.png", object);
  object.myPosition = {0, 0, -4};
  object.myRotation = {0.5, 0, 0};
  object.myRotationSpeed = {0.5, 0, 0};

  objects.push_back(object);

  ObjectProperties object2;
  loadMesh(":/models/cube.obj", object2);
  loadTextures(":/textures/rug_logo.png", object2);
  object2.myPosition = {4, 7, -8};
  object2.myRotation = {0, 0, 0};

  objects.push_back(object2);
  object.myPosition = {-4, 4, -8};
  object2.myPosition = {0,1,-8};
  objects.push_back(object);
  objects.push_back(object2);


  // Initialize transformations
  updateProjectionTransform();

  // Start the timer for 60 updates per second
  timer.start(1000.0 / 60.0);
}

void MainView::createShaderProgram() {
  // Create Normal Shader program
  normalShaderProgram.addShaderFromSourceFile(
      QOpenGLShader::Vertex, ":/shaders/vertshader_normal.glsl");
  normalShaderProgram.addShaderFromSourceFile(
      QOpenGLShader::Fragment, ":/shaders/fragshader_normal.glsl");
  normalShaderProgram.link();

  // Create Gouraud Shader program
  gouraudShaderProgram.addShaderFromSourceFile(
      QOpenGLShader::Vertex, ":/shaders/vertshader_gouraud.glsl");
  gouraudShaderProgram.addShaderFromSourceFile(
      QOpenGLShader::Fragment, ":/shaders/fragshader_gouraud.glsl");
  gouraudShaderProgram.link();

  // Create Phong Shader program
  phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                             ":/shaders/vertshader_phong.glsl");
  phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                             ":/shaders/fragshader_phong.glsl");
  phongShaderProgram.link();

  // Get the uniforms for the normal shader.
  uniformModelViewTransformNormal =
      normalShaderProgram.uniformLocation("modelViewTransform");
  uniformProjectionTransformNormal =
      normalShaderProgram.uniformLocation("projectionTransform");
  uniformNormalTransformNormal =
      normalShaderProgram.uniformLocation("normalTransform");

  // Get the uniforms for the gouraud shader.
  uniformModelViewTransformGouraud =
      gouraudShaderProgram.uniformLocation("modelViewTransform");
  uniformProjectionTransformGouraud =
      gouraudShaderProgram.uniformLocation("projectionTransform");
  uniformNormalTransformGouraud =
      gouraudShaderProgram.uniformLocation("normalTransform");
  uniformMaterialGouraud = gouraudShaderProgram.uniformLocation("material");
  uniformLightPositionGouraud =
      gouraudShaderProgram.uniformLocation("lightPosition");
  uniformLightColourGouraud =
      gouraudShaderProgram.uniformLocation("lightColour");
  uniformTextureSamplerGouraud =
      gouraudShaderProgram.uniformLocation("textureSampler");

  // Get the uniforms for the phong shader.
  uniformModelViewTransformPhong =
      phongShaderProgram.uniformLocation("modelViewTransform");
  uniformProjectionTransformPhong =
      phongShaderProgram.uniformLocation("projectionTransform");
  uniformNormalTransformPhong =
      phongShaderProgram.uniformLocation("normalTransform");
  uniformMaterialPhong = phongShaderProgram.uniformLocation("material");
  uniformLightPositionPhong =
      phongShaderProgram.uniformLocation("lightPosition");
  uniformLightColourPhong = phongShaderProgram.uniformLocation("lightColour");
  uniformTextureSamplerPhong =
      phongShaderProgram.uniformLocation("textureSampler");
}

void MainView::loadMesh(QString name, ObjectProperties &object) {
  Model model(name);
  model.unitize();
  object.myMeshData = model.getVNTInterleaved();

  object.numVertices = model.getVertices().size();

  // Generate VBO/VAO
  glGenBuffers(1, &object.myVBO);
  glGenVertexArrays(1, &object.myVAO);

  // Bind VBO/VAO
  glBindVertexArray(object.myVAO);
  glBindBuffer(GL_ARRAY_BUFFER, object.myVBO);

  // Write the data to the buffer
  glBufferData(GL_ARRAY_BUFFER, object.myMeshData.size() * sizeof(float),
               object.myMeshData.data(), GL_STATIC_DRAW);

  // Set vertex coordinates to location 0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  // Set vertex normals to location 1
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Set vertex texture coordinates to location 2
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
}

void MainView::loadTextures(QString textureName, ObjectProperties &object) {
  glGenTextures(1, &object.myTextureID);
  loadTexture(textureName, object.myTextureID);
}

void MainView::loadTexture(QString file, GLuint texturePtr) {
  // Set texture parameters.
  glBindTexture(GL_TEXTURE_2D, texturePtr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // Push image data to texture.
  QImage image(file);
  QVector<quint8> imageData = imageToBytes(image);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(), 0,
               GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());
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

  // Choose the selected shader.

  for (auto &object : objects) {
    QOpenGLShaderProgram *shaderProgram;
    updateModelPosition(object);
    updateModelTransforms(object);
    switch (currentShader) {
    case NORMAL:
      shaderProgram = &normalShaderProgram;
      shaderProgram->bind();
      updateNormalUniforms();
      break;
    case GOURAUD:
      shaderProgram = &gouraudShaderProgram;
      shaderProgram->bind();
      updateGouraudUniforms();
      break;
    case PHONG:
      shaderProgram = &phongShaderProgram;
      shaderProgram->bind();
      updatePhongUniforms();
      break;
    }

    // Set the texture and draw the mesh.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, object.myTextureID);

    glBindVertexArray(object.myVAO);
    glDrawArrays(GL_TRIANGLES, 0, object.numVertices);
    //qDebug() << "Object rotation: " <<  object.myRotation;
    shaderProgram->release();
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

void MainView::updateNormalUniforms() {
  glUniformMatrix4fv(uniformProjectionTransformNormal, 1, GL_FALSE,
                     projectionTransform.data());
  glUniformMatrix4fv(uniformModelViewTransformNormal, 1, GL_FALSE,
                     meshTransform.data());
  glUniformMatrix3fv(uniformNormalTransformNormal, 1, GL_FALSE,
                     meshNormalTransform.data());
}

void MainView::updateGouraudUniforms() {
  glUniformMatrix4fv(uniformProjectionTransformGouraud, 1, GL_FALSE,
                     projectionTransform.data());
  glUniformMatrix4fv(uniformModelViewTransformGouraud, 1, GL_FALSE,
                     meshTransform.data());
  glUniformMatrix3fv(uniformNormalTransformGouraud, 1, GL_FALSE,
                     meshNormalTransform.data());

  glUniform4fv(uniformMaterialGouraud, 1, &material[0]);
  glUniform3fv(uniformLightPositionGouraud, 1, &lightPosition[0]);
  glUniform3fv(uniformLightColourGouraud, 1, &lightColour[0]);

  // glUniform1i(uniformTextureSamplerGouraud, 0); // Redundant now, but useful
  // when you have multiple textures.
}

void MainView::updatePhongUniforms() {
  glUniformMatrix4fv(uniformProjectionTransformPhong, 1, GL_FALSE,
                     projectionTransform.data());
  glUniformMatrix4fv(uniformModelViewTransformPhong, 1, GL_FALSE,
                     meshTransform.data());
  glUniformMatrix3fv(uniformNormalTransformPhong, 1, GL_FALSE,
                     meshNormalTransform.data());

  glUniform4fv(uniformMaterialPhong, 1, &material[0]);
  glUniform3fv(uniformLightPositionPhong, 1, &lightPosition[0]);
  glUniform3f(uniformLightColourPhong, lightColour.x(), lightColour.y(),
              lightColour.z());

  glUniform1i(uniformTextureSamplerPhong, 0);
}

void MainView::updateProjectionTransform() {
  float aspect_ratio =
      static_cast<float>(width()) / static_cast<float>(height());
  projectionTransform.setToIdentity();
  projectionTransform.perspective(60, aspect_ratio, 0.2f, 20);
}

void MainView::updateModelTransforms(ObjectProperties &object) {
  meshTransform.setToIdentity();
  meshTransform.rotate(QQuaternion::fromEulerAngles(cameraFactor));
  meshTransform.translate(object.myPosition);
  updateUniversalTranslation(object);
  meshTransform.scale(object.scale);

  // If the rotation toggle is on, rotate constantly
  if (rotationToggle)
    object.myRotation.setY(object.myRotation.y() + 0.5f);

  object.myRotation += object.myRotationSpeed;
  meshTransform.rotate(QQuaternion::fromEulerAngles(object.myRotation));
  meshNormalTransform = meshTransform.normalMatrix();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers() {
  for (auto object : objects) {
    glDeleteBuffers(1, &object.myVBO);
    glDeleteVertexArrays(1, &object.myVAO);
  }
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
  for (auto object : objects) {
    object.myRotation = {static_cast<float>(rotateX), static_cast<float>(rotateY),
                static_cast<float>(rotateZ)};
  }
}

void MainView::setScale(int newScale) {
  for (auto object : objects) {
    object.scale = static_cast<float>(newScale) / 100.f;
    updateModelTransforms(object);
  }
}

void MainView::rotateAroundOrigin(float rotateX, float rotateY){
  // TODO Implement mouse movement click and drag
}

void MainView::setShadingMode(ShadingMode shading) {
  qDebug() << "Changed shading to" << shading;
  currentShader = shading;
}

void MainView::setRotationToggle(bool toggleOn) {
  qDebug() << "Change rotation toggle to " << toggleOn;
  rotationToggle = toggleOn;
}

void MainView::updateModelPosition(ObjectProperties &object){
  updateModelSpeed(object);
  object.myPosition += object.speeds;
}

void MainView::updateModelSpeed(ObjectProperties &object){
  object.speeds -= {0.0f, 0.0005f, 0.0f};
  if(object.myPosition.y() <= 0){
      object.speeds.setY(object.speeds.y()*-1);
  }
}

void MainView::updateUniversalTranslation(ObjectProperties &object){
  object.myPosition += movementFactor;
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
