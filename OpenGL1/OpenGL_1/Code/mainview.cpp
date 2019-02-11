#include "mainview.h"
#include "math.h"
#include "vertex.h"

#include <QDateTime>

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
    glDeleteBuffers(1,&VBO_Cube);
    glDeleteBuffers(1,&VBO_Pyramid);

    // Free VAOs
    glDeleteVertexArrays(1,&VAO_Cube);
    glDeleteVertexArrays(1,&VAO_Pyramid);
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
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
             this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // Set the color of the screen to be black on clear (new frame)
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);

    createShaderProgram();

    // Load the two shapes
    loadCube();
    loadPyramid();

    // Set the initial position
    setInitialTranslation();
    setInitialProjection();
}

void MainView::createShaderProgram()
{
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader.glsl");

    uniformTransform = shaderProgram.uniformLocation("modelTransform");
    uniformProjection = shaderProgram.uniformLocation("modelProjection");

    shaderProgram.link();
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
    glUniformMatrix4fv(uniformProjection,1,GL_FALSE,projectionTransform.data());

    // Set the transform location for the cube
    glUniformMatrix4fv(uniformTransform,1,GL_FALSE,cubeTransform.data());
    glBindVertexArray(VAO_Cube);
    glDrawArrays(GL_TRIANGLES,0,36);

    // Set the transform location for the pyramid
    glUniformMatrix4fv(uniformTransform,1,GL_FALSE,pyramidTransform.data());
    glBindVertexArray(VAO_Pyramid);
    glDrawArrays(GL_TRIANGLES,0,18);

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
void MainView::resizeGL(int newWidth, int newHeight) 
{
    // TODO: Update projection to fit the new aspect ratio
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)

    aspectRatio = static_cast<float>(width()) / static_cast<float>(height());
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";
    Q_UNIMPLEMENTED();
}

void MainView::setScale(int scale)
{
    qDebug() << "Scale changed to " << scale;
    Q_UNIMPLEMENTED();
}

void MainView::setShadingMode(ShadingMode shading)
{
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
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}

// Creates and loads a unit cube into the GPU buffer
void MainView::loadCube(){
    cubeVertexCount = 36;

    // This defines the cube to be rendered
    Vertex cube[36] = {
        {{-1,-1,-1},{1,0,1}}, // Front
        {{1,-1,-1},{0,1,0}},
        {{-1,1,-1},{0,0,1}},
        {{-1,1,-1},{1,0,0}},
        {{1,-1,-1},{0,0,0}},
        {{1,1,-1},{1,1,1}},
        {{-1,-1,1},{0,0,1}}, // Back
        {{1,-1,1},{1,1,0}},
        {{-1,1,1},{0,0,1}},
        {{-1,1,1},{0,1,0}},
        {{1,-1,1},{1,0,0}},
        {{1,1,1},{1,1,1}},
        {{-1,1,-1},{0,0,0}}, // Top
        {{1,1,-1},{1,0,1}},
        {{1,1,1},{1,0,0}},
        {{1,1,1},{0,1,0}},
        {{-1,1,1},{1,1,1}},
        {{-1,1,-1},{1,0,1}},
        {{-1,-1,-1},{0,1,1}}, // Bottom
        {{1,-1,-1},{1,1,0}},
        {{1,-1,1},{0,1,0}},
        {{1,-1,1},{1,1,1}},
        {{-1,-1,1},{0,0,1}},
        {{-1,-1,-1},{1,1,0}},
        {{-1,1,-1},{1,0,0}}, // Left
        {{-1,-1,-1},{0,1,0}},
        {{-1,-1,1},{0,0,1}},
        {{-1,-1,1},{1,0,0}},
        {{-1,1,1},{1,1,0}},
        {{-1,1,-1},{0,1,1,}},
        {{1,1,-1},{1,1,1}}, // Right
        {{1,-1,-1},{0,1,1}},
        {{1,-1,1},{0,1,0}},
        {{1,-1,1},{1,1,1}},
        {{1,1,1},{0,1,0}},
        {{1,1,-1},{1,1,0}},
    };

    // Generate the buffer and vertex array, and return the IDs to upload the cube to
    glGenBuffers(1,&VBO_Cube);
    glGenVertexArrays(1,&VAO_Cube);

    // Now send the vertices(cube) to the GPU vind bind
    glBindVertexArray(VAO_Cube);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_Cube);
    glBufferData(GL_ARRAY_BUFFER,cubeVertexCount*(sizeof(Vertex)),cube,GL_STATIC_DRAW);

    // Now inform the GPU what attributes to use for received arrays
    // via the contents of the shader files
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Finally inform the layout of the data for the attributes
    // With OFFSET equal to the size of the coordinate array
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex), reinterpret_cast<void*>(sizeof(Vertex::coordinates)));
}

void MainView::loadPyramid(){
    pyramidVertexCount = 18;

    // This defines the cube to be rendered
    Vertex pyramid[18] = {
        {{-1,-1,-1},{0,1,1}}, // Bottom
        {{1,-1,-1},{1,1,0}},
        {{1,-1,1},{0,1,0}},
        {{1,-1,1},{1,1,1}},
        {{-1,-1,1},{0,0,1}},
        {{-1,-1,-1},{1,1,0}},
        {{-1,-1,-1},{1,0,0}}, // Front
        {{1,-1,-1},{1,0,1}},
        {{0,1,0},{0,1,0}},
        {{-1,-1,1},{0,0,1}}, // Back
        {{1,-1,1},{1,1,1}},
        {{0,1,0},{0,0,0}},
        {{-1,-1,1},{1,1,0}}, // Left
        {{-1,-1,-1},{0,1,1}},
        {{0,1,0},{1,0,1}},
        {{1,-1,-1},{0,0,1}}, // Right
        {{1,-1,1},{1,0,0}},
        {{0,1,0},{1,0,1}},
    };

    // Generate the buffer and vertex array, and return the IDs to upload the cube to
    glGenBuffers(1,&VBO_Pyramid);
    glGenVertexArrays(1,&VAO_Pyramid);

    // Now send the vertices(cube) to the GPU vind bind
    glBindVertexArray(VAO_Pyramid);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_Pyramid);
    glBufferData(GL_ARRAY_BUFFER,pyramidVertexCount*(sizeof(Vertex)),pyramid,GL_STATIC_DRAW);

    // Now inform the GPU what attributes to use for received arrays
    // via the contents of the shader files
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Finally inform the layout of the data for the attributes
    // With OFFSET equal to the size of the coordinate array
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex), reinterpret_cast<void*>(sizeof(Vertex::coordinates)));
}

void MainView::setInitialTranslation(){
    // Initialize as identify matrix
    cubeTransform.setToIdentity();
    pyramidTransform.setToIdentity();

    // Translate to beginig position (2,0,-6) and (-2,0,-6)
    //cubeTransform.translate(0,0,0);
    //pyramidTransform.translate(0,0,0);
}

void MainView::setInitialProjection(){
    // This sets the POV and position of camera initial camera
    //aspectRatio = static_cast<float>(width()) / static_cast<float>(height());

    // Set perspective
    projectionTransform.setToIdentity();
    //projectionTransform.perspective(60.0, aspectRatio, 0.0, 40.0);
}
