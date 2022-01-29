#include "camera.h"

Camera::Camera()
{
    isPerspective = true;
    modelViewChanged = true;
    perspectiveChanged = true;
    fov = 45.0f;
    nearPlane = 0.1f;
    farPlane = 100.0f;

    // orthographic camera parameters
    orthoLeft = -200.0f;
    orthoRight = 200.0f;
    orthoBottom = -200.0f;
    orthoTop = 200.0f;

    reset();
}

void Camera::set(QVector3D eye, QVector3D center, QVector3D up)
{
    modelViewChanged = true;

    this->eye = eye;
    this->center = center;
    this->up = up;
    emit valueChanged();
}

void Camera::setFov(float fov)
{
    perspectiveChanged = true;

    this->fov = fov;
    emit valueChanged();
}

void Camera::setNear(float near)
{
    perspectiveChanged = true;
    nearPlane = near;
    emit valueChanged();
}

void Camera::setFar(float far)
{
    perspectiveChanged = true;
    farPlane = far;
    emit valueChanged();
}

void Camera::setScreenRatio(float ratio)
{
    perspectiveChanged = true;
    screenRatio = ratio;
    emit valueChanged();
}

void Camera::reset()
{
    modelViewChanged = true;

    eye = QVector3D(0,0,1);
    center = QVector3D(0,0,0);
    up = QVector3D(0,1,0);
    emit valueChanged();
}

void Camera::move(QVector3D vec)
{
    modelViewChanged = true;

    eye += vec;
    center += vec;
    emit valueChanged();
}

void Camera::rotate(QMatrix4x4 m)
{
    modelViewChanged = true;

    QVector3D viewVec = center - eye;
    QVector3D newViewVec = m * viewVec;
    center = eye + newViewVec;

    up = m * up;
    emit valueChanged();
}

void Camera::rotateAroundRight(float angle)
{
    modelViewChanged = true;

    QMatrix4x4 m;
    m.rotate(angle, getRight());
    rotate(m);
    emit valueChanged();
}

void Camera::rotateAroundUp(float rotation)
{
    modelViewChanged = true;

    QMatrix4x4 m;
    m.rotate(rotation, up);
    rotate(m);
    emit valueChanged();
}


QVector3D Camera::viewDirection()
{
    return (center - eye).normalized();
}

QMatrix4x4 Camera::getModelViewMatrix()
{
    if(modelViewChanged)
    {
        modelView.setToIdentity();
        modelView.lookAt(eye, center, up);
        modelViewChanged = false;
    }

    return modelView;
}

QMatrix4x4 Camera::getProjectionMatrix()
{
    if(perspectiveChanged)
    {
        projection.setToIdentity();
        if(isPerspective) {
            projection.perspective(fov, screenRatio, nearPlane, farPlane);
        }
        else
        {
            projection.ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, nearPlane, farPlane);
        }
        perspectiveChanged = false;
    }
    return projection;
}

QVector3D Camera::getUp()
{
    return up;
}

QVector3D Camera::getRight()
{
    return QVector3D::crossProduct(viewDirection(), up);
}

// ToDo: invalidates eye, center, up: shall we remove this method?
void Camera::setModelView(QMatrix4x4 modelView)
{
    this->modelView = modelView;
    emit valueChanged();
}

// ToDo: invalidates fov, screenRatio, nearPlane, farPlane: shall we remove this method?
void Camera::setProjection(QMatrix4x4 projection)
{
    this->projection = projection;
    emit valueChanged();
}
