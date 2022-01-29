#ifndef CAMERA_H
#define CAMERA_H

#include "nn_global.h"
#include <QVector3D>
#include <QObject>
#include <QQuaternion>
#include <QRect>
#include <QMatrix4x4>

class NN_API Camera : public QObject
{
    Q_OBJECT

public:
    Camera();

    void reset();
    void move(QVector3D vec);
    void rotate(QMatrix4x4 m);
    void rotateAroundRight(float angle);
    void rotateAroundUp(float angle);

    void set(QVector3D eye, QVector3D center, QVector3D up);

    void setFov(float fov);
    void setNear(float near);
    void setFar(float far);
	void setScreenRatio(float ratio);

    float getFov(){return fov;}
    float getNear(){return nearPlane;}
    float getFar(){return farPlane;}

    void setModelView();
    void setProjection();

    QVector3D getUp();
    QVector3D getRight();

    QVector3D viewDirection();

    QMatrix4x4 getModelViewMatrix();
    QMatrix4x4 getProjectionMatrix();

    void makePerspective(){isPerspective = true;}
    void makeOrthographic(){isPerspective = false;}

    // ToDo: invalidates eye, center, up: shall we remove this method?
    void setModelView(QMatrix4x4 modelView);
    // ToDo: invalidates fov, screenRatio, nearPlane, farPlane: shall we remove this method?
    void setProjection(QMatrix4x4 projection);

signals:
     void valueChanged();

private:

    bool modelViewChanged;
    bool perspectiveChanged;

    bool isPerspective;
    float orthoLeft;
    float orthoRight;
    float orthoBottom;
    float orthoTop;

    float fov;
    float screenRatio;
    float nearPlane;
    float farPlane;
    QRect viewport;

    QVector3D eye;
    QVector3D center;
    QVector3D up;

    QMatrix4x4 modelView;
    QMatrix4x4 projection;
};

#endif // CAMERA_H
