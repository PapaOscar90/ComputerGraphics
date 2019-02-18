#include "mainview.h"
#include "math.h"
#include "Vertex.h"

#include <QDateTime>
#include <QOpenGLShaderProgram>

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

MainView::~MainView() {
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";
}

void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);    

    initializeShaders();
    loadTriangle();
}

void MainView::resizeGL(int newWidth, int newHeight) {

    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
}

void MainView::paintGL() {
    // Bitwise or to clear color and depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the background color to black
    glClearColor(0,0,0,1);

    // Bind the program and the VAO
    program.bind();
    glBindVertexArray(vertexArrayID);

    // Perform draw
    glDrawArrays(GL_TRIANGLES,0,3);

    // Unbind
    program.release();
}

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}

void MainView::initializeShaders(){
    // Add shaders from source files in resource
    program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");
    program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    program.link();
}

void MainView::loadTriangle(){
    // Declare the triangle via 3 vertices with color
    Vertex triangle[3] = {{{0,1},{1,0,0}},
                          {{-1,-1},{0,1,0}},
                          {{1,-1},{0,0,1}}};

    // Generate buffers for VBO and VAO
    glGenBuffers(1,&vertexBufferID);
    glGenVertexArrays(1,&vertexArrayID);

    // Send data to GPU via binding buffer and vertex array
    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER,3*(sizeof(Vertex)),triangle,GL_STATIC_DRAW);

    // Inform the GPU how to use the data that was just transferred
    // Enable the attribute arrays
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Tell the GPU how the data is shaped via the number of and size of internal structs
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex), reinterpret_cast<void*> (sizeof(Vertex::coordinates)));
}
