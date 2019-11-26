#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float msec)	{
	float speed = msec * 5.0f;
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F)) {
		freeMove = !freeMove;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_X)) {
		std::cout << "X: " << position.x << ", Y: " << position.y << ", Z:" << position.z << endl;
	}
	
	if (freeMove) {
		//Update the mouse by how much
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);

		//Bounds check the pitch, to be between straight up and straight down ;)
		pitch = min(pitch, 90.0f);
		pitch = max(pitch, -90.0f);

		if (yaw < 0) {
			yaw += 360.0f;
		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}

		if (Window::GetKeyboard()->KeyHeld(KEYBOARD_CONTROL))
			speedDiv = 2;
		else
			speedDiv = 5;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * speed / speedDiv;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * speed / speedDiv;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * speed / speedDiv;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * speed / speedDiv;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_R)) {
			roll = 0;
		}

		//Roll
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q)) {
			roll -= 2.0f;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_E)) {
			roll += 2.0f;
		}


		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			position.y -= speed / speedDiv;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			position.y += speed / speedDiv;
		}
	} else {
		tim += msec;
		tim2 += msec;
		clp2 = min(1, max((tim2 / (4000 * (points.size()/look.size()))), 0));
		if (clp2 != 0 && clp2 == 1 && flPoint < look.size() - 2) {
			clp2 = 0;
			tim2 = 0;
			flPoint++;
		}

		float clp = min(1, max((tim / 4000), 0));
		
		if (clp == 1 && point < points.size() - 2) {
			tim = 0;
			clp = 0;
			point++;
		}
		if (point < points.size() - 2) {
			if (point == 0)
				PrevEnd = Vector3(position.x - 10, position.y, position.z);
			else
				PrevEnd = points[point - 1];
			Vector3 p0 = PrevEnd;
			Vector3 p1 = points[point];
			Vector3 p2 = points[point + 1];
			Vector3 p3 = points[point + 2];
			



			position = BCurved(p0, p1, p2, p3, clp);
		}
	}
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix()	{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important

	if(freeMove || miniMap)
		return	Matrix4::Rotation(-pitch, Vector3(1,0,0)) * Matrix4::Rotation(-yaw, Vector3(0,1,0)) * Matrix4::Rotation(-roll, Vector3(0,0,1)) * Matrix4::Translation(-position);
	if (flPoint == look.size() - 2)
		flPoint--;
	if (flPoint == 0)
		PrevEnd = Vector3(look[0].x + 10, look[0].y, look[0].z);
	else {
		PrevEnd = look[flPoint- 1];
	}
	
	Vector3 l1 = (flPoint == look.size() - 3) ? lastPos : look[flPoint];
	Vector3 l2 = look[flPoint  + 1];
	Vector3 l3 = look[flPoint  + 2];
	lastPos = BCurved(PrevEnd, l1, l2, l3, clp2);
	return Matrix4::BuildViewMatrix(position, lastPos);
};

Vector3 Camera::BCurved(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t) {
	float t01 = pow(Vector3::distance(p0, p1), alpha);
	float t12 = pow(Vector3::distance(p1, p2), alpha);
	float t23 = pow(Vector3::distance(p2, p3), alpha);

	Vector3 m1 = (((p1 - p0) / t01 - (p2 - p0) / (t01 + t12)) * t12 + p2 - p1) * (1.0f - tension);
	Vector3 m2 = (((p3 - p2) / t23 - (p3 - p1) / (t12 + t23)) * t12 + p2 - p1) * (1.0f - tension);

	Vector3 a = (p1 - p2) * 2 + m1 + m2;
	Vector3 b = (p1 - p2) * -3 - m1 - m1 - m2;
	Vector3 c = m1;
	Vector3 d = p1;

	return a * pow(t, 3) + b * pow(t, 2) + c * t + d;
}


