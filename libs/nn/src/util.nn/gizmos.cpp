#include "gizmos.h"

QOpenGLFunctions_4_3_Core *Gizmos::gl = NULL;
NNShader *Gizmos::linePointshader = NULL;
int Gizmos::vertices_size = 0;
unsigned int Gizmos::linePointVAO[1] = {0};
unsigned int Gizmos::linePointVBO[1] = {0};
QVector<QVector3D> Gizmos::myVertices;
unsigned int Gizmos::queryID[2] = {};
QElapsedTimer Gizmos::elapsedTimer;
bool Gizmos::initialized = false;

void Gizmos::setupVertexBuffers()
{
    qDebug() << "Gizmos::setupVertexBuffers.";
    linePointshader = new NNShader("shaders/rendering/linePoint.vs", "shaders/rendering/linePoint.fs");
    if(!gl)
    {
        qDebug() << "Gizmos: QOpenGLFunctions are not initialized!";
        return;
    }


    gl->glGenVertexArrays(1, &linePointVAO[0]);
    gl->glGenBuffers(1, &linePointVBO[0]);

    gl->glBindVertexArray(linePointVAO[0]);
    gl->glBindBuffer(GL_ARRAY_BUFFER, linePointVBO[0]);

    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, // same as in glEnableVertexAttribArray(0)
                          3, // # of coordinates that build a vertex
                          GL_FLOAT, // data type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void*)0);// vbo offset


    gl->glBindVertexArray(0);
	initialized = true;
}


void Gizmos::uploadGrid(float cellSize, QVector3D gridSize)
{
    QVector<QVector3D> vertices;

    float maxValueX = gridSize.x() * cellSize;
    float minValueX = -maxValueX;

    float maxValueY = gridSize.y() * cellSize;
    float minValueY = -maxValueY;

    float maxValueZ = gridSize.z() * cellSize;
    float minValueZ = -maxValueZ;


    for(float x = minValueX; x <= maxValueX; x += (cellSize*10))
    {
        for(float y = minValueY; y <= maxValueY; y += (cellSize*10))
        {
            QVector3D lineStart(x, y, minValueZ);
            QVector3D lineEnd(x, y, maxValueZ);

            vertices.push_back(lineStart);
            vertices.push_back(lineEnd);
        }
    }

    for(float x = minValueX; x <= maxValueX; x += (cellSize*10))
    {
        for(float z = minValueZ; z <= maxValueZ; z += (cellSize*10))
        {
            QVector3D lineStart(x, minValueY, z);
            QVector3D lineEnd(x, maxValueY, z);

            vertices.push_back(lineStart);
            vertices.push_back(lineEnd);
        }
    }

    for(float y = minValueY; y <= maxValueY; y += (cellSize*10))
    {
        for(float z = minValueZ; z <= maxValueZ; z += (cellSize*10))
        {
            QVector3D lineStart(minValueX, y, z);
            QVector3D lineEnd(maxValueX, y, z);

            vertices.push_back(lineStart);
            vertices.push_back(lineEnd);
        }
    }

    Gizmos::setVertices(vertices);
}

void Gizmos::drawWireMesh()
{

}

void Gizmos::uploadLines(std::vector<QVector3D> vecStart, std::vector<QVector3D> vecEnd)
{
    if(vecStart.size() != vecEnd.size() )
    {
        qDebug() << "Gizmos::uploadLine: vecStart and vecEnd do not have the same size.";
        return;
    }
    QVector<QVector3D> vertices;
    for (int i = 0; i < vecStart.size(); ++i)
    {
        vertices.push_back(vecStart[i]);
        vertices.push_back(vecEnd[i]);
    }
    Gizmos::setVertices(vertices);
}

void Gizmos::uploadLines(int numVecs, GLuint bufferVecsStart, GLuint bufferVecs)
{
    GLint bufferSize = numVecs * 4 * sizeof(GLfloat);


	gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferVecsStart);
	GLint maxSize = 0;
	gl->glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &maxSize);
	if(bufferSize > maxSize) {
		qDebug() << "Error: Gizmos::uploadLines: input buffer is too small: " << bufferSize <<" > "" <<  maxSize";
		return;
	}
    float *vecStarts = reinterpret_cast<float *> (gl->glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bufferSize , GL_MAP_READ_BIT));

    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferVecs);
	maxSize = 0;
	gl->glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &maxSize);
	if(bufferSize > maxSize) {
		qDebug() << "Error: Gizmos::uploadLines: input buffer is too small: " << bufferSize <<" > "" <<  maxSize";
		return;
	}
    float *vecs = reinterpret_cast<float *> (gl->glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bufferSize , GL_MAP_READ_BIT));

    QVector<QVector3D> vertices;
    for (int i = 0; i < numVecs; ++i)
    {
        QVector3D vecStart = QVector3D(vecStarts[i * 4], vecStarts[i * 4 +1], vecStarts[i * 4 + 2]);
        QVector3D vec = QVector3D(vecs[i * 4], vecs[i * 4 +1], vecs[i * 4 + 2]);

        vertices.push_back(vecStart);
        vertices.push_back(vecStart + vec * 20);
    }

    Gizmos::setVertices(vertices);

    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferVecsStart);
    gl->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferVecs);
    gl->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Gizmos::setVertices(QVector<QVector3D> &vertices)
{
    if(!initialized) {
        Gizmos::setupVertexBuffers();
    }
    //qDebug() << "Gizmos::setVertices.";
    gl->glBindBuffer(GL_ARRAY_BUFFER, linePointVBO[0]);
    gl->glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
    vertices_size = vertices.size();

    myVertices.clear();
    for (int i = 0; i < vertices.size(); ++i) {
        myVertices.push_back(vertices[i]);
    }
    //myVertices.clear();
    //qCopy(vertices.begin(), vertices.end(), myVertices.begin());
}

void Gizmos::drawLines(PFBO targetFbo, QMatrix4x4 mvm, QMatrix4x4 pm, float scale, float lineWidth)
{
    if(vertices_size <= 0) {
        return;
    }

    gl->glBindBuffer(GL_ARRAY_BUFFER, linePointVBO[0]);
    gl->glBufferData(GL_ARRAY_BUFFER, myVertices.size() * 3 * sizeof(GLfloat), &myVertices[0], GL_STATIC_DRAW);

    //qDebug() << "vertices_size: " << vertices_size;
    //qDebug() << "linePointVAO[0]: " << linePointVAO[0];

    targetFbo->bind();
    gl->glDrawBuffer(GL_COLOR_ATTACHMENT0);
    gl->glClearColor(0.2, 0.3, 0.2, 1.0);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glDisable(GL_DEPTH_TEST);
    gl->glLineWidth(lineWidth);
    gl->glEnable(GL_LINE_SMOOTH);
    gl->glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);

    auto prog = linePointshader->getShader();
    prog->bind();
    prog->setUniformValue("modelViewMatrix", mvm);
    prog->setUniformValue("projectionMatrix", pm);
    prog->setUniformValue("scale", scale);
    gl->glBindVertexArray(linePointVAO[0]);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, // same as in glEnableVertexAttribArray(0)
                          3, // # of coordinates that build a vertex
                          GL_FLOAT, // data type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void*)0);// vbo offset


    //glEnableClientState(GL_VERTEX_ARRAY);
    //int offset = 0;
    //glVertexPointer(3, GL_FLOAT, vertices.size() * 3 * sizeof(GLfloat), (const void *) offset);


    gl->glDrawArrays(GL_LINES, 0, vertices_size);

    //~ cleanup
    prog->release();
    targetFbo->release();
}

void Gizmos::startGPUTimer()
{
    #ifdef BENCHMARKING
    gl->glGenQueries(2, queryID);
    gl->glQueryCounter(queryID[0], GL_TIMESTAMP);
    #endif
}

void Gizmos::printGPUTimer(QString msg)
{
    #ifdef BENCHMARKING
    GLuint64 startTime, stopTime;
    gl->glQueryCounter(queryID[1], GL_TIMESTAMP);
    // wait until the results are available
    GLint stopTimerAvailable = 0;
    while (!stopTimerAvailable) {
        gl->glGetQueryObjectiv(queryID[1], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
    }

    // get query results
    gl->glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &startTime);
    gl->glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, &stopTime);

    qDebug() << msg << " (GPU) took: " << ((stopTime - startTime) / 1000000.0) << " ms";

    #endif
}

void Gizmos::startCPUTimer()
{
    #ifdef BENCHMARKING
    elapsedTimer.start();
    #endif
}

void Gizmos::printCPUTimer(QString msg)
{
    #ifdef BENCHMARKING
    qDebug() << msg << " (CPU) took" << elapsedTimer.elapsed() << "ms";
    #endif
}

void Gizmos::printBufferInt(GLuint buffer, int numPoints, QString name)
{
    auto bufferSize = numPoints * sizeof(GLuint);
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    GLint maxSize = 0;
    gl->glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &maxSize);
    if(bufferSize > maxSize) {
        bufferSize = maxSize;
        numPoints = bufferSize / sizeof(int);
    }

    auto result = reinterpret_cast<int *> (gl->glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bufferSize , GL_MAP_READ_BIT));

    for (int i = 0; i < numPoints; ++i)
    {
        qDebug() << name <<  " [" << i << "]:" << result[i];
    }
    gl->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void Gizmos::printBufferFloat(GLuint buffer, int numPoints, QString name)
{
    auto bufferSize = numPoints * sizeof(GLuint);
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    GLint maxSize = 0;
    gl->glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &maxSize);
    if(bufferSize > maxSize) {
        bufferSize = maxSize;
        numPoints = bufferSize / sizeof(float);
    }
    auto result = reinterpret_cast<float *> (gl->glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bufferSize , GL_MAP_READ_BIT));

    for (int i = 0; i < numPoints; ++i)
    {
        qDebug() << name <<  " [" << i << "]:" << QLocale(QLocale::English).toString(result[i]);
    }
    gl->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void Gizmos::printBufferVec4(GLuint buffer, int numPoints, QString name)
{
    auto bufferSize = numPoints * sizeof(QVector4D);
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    GLint maxSize = 0;
    gl->glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &maxSize);
    if(bufferSize > maxSize) {
        bufferSize = maxSize;
        numPoints = bufferSize / sizeof(QVector4D);
    }
    auto result = reinterpret_cast<QVector4D *> (gl->glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bufferSize , GL_MAP_READ_BIT));

    for (int i = 0; i < numPoints; ++i)
    {
        qDebug() << name <<  " [" << i << "]:" << result[i];
    }
    gl->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void Gizmos::printBufferVec3(GLuint buffer, int numPoints, QString name)
{
    auto bufferSize = numPoints * sizeof(QVector3D);
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    GLint maxSize = 0;
    gl->glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &maxSize);
    if(bufferSize > maxSize) {
        bufferSize = maxSize;
        numPoints = bufferSize / sizeof(QVector3D);
    }
    auto result = reinterpret_cast<QVector3D *> (gl->glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bufferSize , GL_MAP_READ_BIT));

    for (int i = 0; i < numPoints; ++i)
    {
        qDebug() << name <<  " [" << i << "]:" << result[i];
    }
    gl->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}
