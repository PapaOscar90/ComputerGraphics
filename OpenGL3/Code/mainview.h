#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QVector3D>
#include <QImage>
#include <QVector>
#include <memory>
#include <QMatrix4x4>

using namespace std;

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger *debugLogger;
    QTimer timer; // timer used for animation

    QOpenGLShaderProgram normalShaderProgram,
                         gouraudShaderProgram,
                         phongShaderProgram;

    // Store the rotation applied to each shape
    QQuaternion rotationFactor = QQuaternion::fromEulerAngles({0.0, 0.0, 0.0});

    // Store the translation applied to each shape
    QVector3D translationFactor = {0,0,0};

    // Store the properties of each object
    struct ObjectProperties {
        QQuaternion myRotation = QQuaternion::fromEulerAngles({0.0, 0.0, 0.0});
        QVector3D myPosition = {0,0,0};
        float mySpeed = 0;
        unsigned int numVertices;
        GLuint myVAO;
        GLuint myVBO;
        QVector<float> myMeshData;
        GLuint myTextureID;
    };

    // Uniforms for the normal shader.
    GLint uniformModelViewTransformNormal;
    GLint uniformProjectionTransformNormal;
    GLint uniformNormalTransformNormal;

    // Uniforms for the gouraud shader.
    GLint uniformModelViewTransformGouraud;
    GLint uniformProjectionTransformGouraud;
    GLint uniformNormalTransformGouraud;

    GLint uniformMaterialGouraud;
    GLint uniformLightPositionGouraud;
    GLint uniformLightColourGouraud;

    GLint uniformTextureSamplerGouraud;

    // Uniforms for the phong shader.
    GLint uniformModelViewTransformPhong;
    GLint uniformProjectionTransformPhong;
    GLint uniformNormalTransformPhong;

    GLint uniformMaterialPhong;
    GLint uniformLightPositionPhong;
    GLint uniformLightColourPhong;

    GLint uniformTextureSamplerPhong;

    // Buffers
    GLuint meshVAO;
    GLuint meshVBO;
    GLuint meshSize;


    // Texture
    GLuint texturePtr;

    // Transforms
    float scale = 1.f;
    QVector3D rotation;
    QMatrix4x4 projectionTransform;
    QMatrix3x3 meshNormalTransform;
    QMatrix4x4 meshTransform;

    // Phong model constants.
    QVector4D material = {0.5, 0.5, 1, 5};
    QVector3D lightPosition = {1, 100, 1};
    QVector3D lightColour = {1, 1, 1};

public:
    enum ShadingMode : GLuint
    {
        PHONG = 0, NORMAL, GOURAUD
    };

    MainView(QWidget *parent = 0);
    ~MainView();

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int scale);
    void setShadingMode(ShadingMode shading);
    void setRotationToggle(bool toggleOn);

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
    void onMessageLogged( QOpenGLDebugMessage Message );

private:
    vector<ObjectProperties> objects;

    void createShaderProgram();
    void loadMesh(QString fileName, ObjectProperties *object);

    // Loads texture data into the buffer of texturePtr.
    void loadTextures(ObjectProperties *object);
    void loadTexture(QString file, GLuint texturePtr);

    void destroyModelBuffers();

    void updateProjectionTransform();
    void updateModelTransforms();

    void updateNormalUniforms();
    void updateGouraudUniforms();
    void updatePhongUniforms();

    void updateRotation();

    // Useful utility method to convert image to bytes.
    QVector<quint8> imageToBytes(QImage image);

    // The current shader to use.
    ShadingMode currentShader = PHONG;

    // Whether the user toggled the rotation on/off
    bool rotationToggle = true;
};

#endif // MAINVIEW_H
