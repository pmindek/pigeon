#include "cameracontroller.h"

#include "input.h"

CameraController::CameraController()
{
	camera = NULL;
	mouseRotateStarted = false;
}

void CameraController::update()
{
	if(!camera)
		return;

	QVector3D velocity;
	if (Input::getKey(Qt::Key::Key_W))
	{
		velocity += QVector3D(0, 0, -1);
	}
	if (Input::getKey(Qt::Key::Key_S))
	{
		velocity += QVector3D(0, 0, 1);
	}
	if (Input::getKey(Qt::Key::Key_A))
	{
		velocity += QVector3D(-1, 0, 0);
	}
	if (Input::getKey(Qt::Key::Key_D))
	{
		velocity += QVector3D(1, 0, 0);
	}

	QVector3D viewDir = camera->viewDirection();

	if(Input::getKey(Qt::Key::Key_Shift))
	{
		velocity *= fastSpeed;
		velocity *= fastSpeed;

	}
	else
	{
		velocity *= slowSpeed;
		velocity *= slowSpeed;
	}

	moveMomentum += velocity;
	//moveMomentum.setX(std::min(moveMomentum.x(), 0.03f));
	//moveMomentum.setY(std::min(moveMomentum.y(), 0.03f));
	//moveMomentum.setZ(std::min(moveMomentum.z(), 0.03f));
	if(moveMomentum.length() < 0.00001f) {
		moveMomentum = QVector3D();
	}

	QVector3D adjustedMomentum = (camera->getModelViewMatrix().inverted() * QVector4D(moveMomentum, 0.0f)).toVector3D();

	camera->move(adjustedMomentum);

	moveMomentum *= 0.87f;


	float zoom = Input::mouseScrollDelta().y();

	QVector3D zoomMove = viewDir * zoom * zoomSpeed;
	camera->move(zoomMove);



	if(Input::getMouseButton(Qt::MouseButton::LeftButton))
	{
		QPoint currentMousePos = Input::mousePosition();
		if(mouseRotateStarted)
		{
			QPoint mouseDelta = lastMousePos - currentMousePos;
			float rotationX = mouseDelta.x() * 0.03f;
			float rotationY = mouseDelta.y() * 0.03f;

			camera->rotateAroundUp(rotationX);
			camera->rotateAroundRight(rotationY);
		}
		else
		{
			mouseRotateStarted = true;
		}

		lastMousePos = currentMousePos;

	}
	if(!Input::getMouseButton(Qt::MouseButton::LeftButton))
	{
		mouseRotateStarted = false;
	}



	if (Input::getKey(Qt::Key::Key_R))
	{
		camera->reset();
		moveMomentum = QVector3D();
	}
}

