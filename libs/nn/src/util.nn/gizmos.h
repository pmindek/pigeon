#ifndef GIZMOS_H
#define GIZMOS_H

/**
 * @brief The Gizmos class
 *
 * Gizmos are used to give visual debugging or setup aids.
 */


#include <QVector>
#include <QVector3D>
#include <QElapsedTimer>
#include <QtOpenGL>
#include <QOpenGLFunctions_4_3_Core>

#include "nn.h"

class NN_API Gizmos
{
public:
    static void setGL(QOpenGLFunctions_4_3_Core *glFunc){gl = glFunc;}
    static void setupVertexBuffers();

    static void uploadGrid(float cellSize, QVector3D gridSize);
    static void drawWireMesh();
	static void drawLines(PFBO targetFbo, QMatrix4x4 mvm, QMatrix4x4 pm, float scale, float lineWidth);
    static void uploadLines(std::vector<QVector3D> vecStart, std::vector<QVector3D> vecEnd);
    static void uploadLines(int numVecs, GLuint bufferVecsStart, GLuint bufferVecs);
    static void setVertices(QVector<QVector3D> &vertices);

    static void printBufferInt(GLuint buffer, int numPoints, QString name);
    static void printBufferFloat(GLuint buffer, int numPoints, QString name);
    static void printBufferVec4(GLuint buffer, int numPoints, QString name);
    static void printBufferVec3(GLuint buffer, int numPoints, QString name);

    static void startGPUTimer();
    static void printGPUTimer(QString msg);

    static void startCPUTimer();
    static void printCPUTimer(QString msg);

private:

    static NNShader *linePointshader;
    static QOpenGLFunctions_4_3_Core *gl;
    static QVector<QVector3D> myVertices;

    static unsigned int linePointVAO[1];
    static unsigned int linePointVBO[1];

    static int vertices_size;

    static unsigned int queryID[2];
    static QElapsedTimer elapsedTimer;

    static bool initialized;

};

#endif // GIZMOS_H
