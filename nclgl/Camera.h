/******************************************************************************
Class:Camera
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:FPS-Style camera. Uses the mouse and keyboard from the Window
class to get movement values!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Window.h"
#include "Matrix4.h"
#include "Vector3.h"

class Camera	{
public:
	Camera(void){
		yaw		= 0.0f;
		pitch	= 0.0f;
		roll	= 0.0f;
		points = std::vector<Vector3>();

		points.push_back(this->position);
		points.push_back(Vector3(3214.54, 1067.01, 3720.28));
		points.push_back(Vector3(6991.49, 1067.01, 9227.1));
		points.push_back(Vector3(11000.6, 883.303, 6547.01));

		points.push_back(Vector3(12449.7, 1234.97, 6552.66));
		points.push_back(Vector3(10775.3, 117.367, 5658.05));
		points.push_back(Vector3(7861.1, 1118.13, 5916.11));

		points.push_back(Vector3(5925.33, 1485.08, 3683.89));
		points.push_back(Vector3(7273.82, 1485.08, 2961.63));
		points.push_back(Vector3(8131.33, 1485.08, 5536.2));

		look.push_back(Vector3(9130.21, 618.146, 6196.4	));
		look.push_back(Vector3(4432.9, 468.027, 6938.92	));
		look.push_back(Vector3(9168.01, 101.058, 5249.43));

		look.push_back(Vector3(7100.36, 302.058, 3940.93));
		look.push_back(Vector3(5032.71, 503.038, 2632.34));
		look.push_back(Vector3(6114.765, 970.054, 3512.41));

		look.push_back(Vector3(7196.82, 1437.07, 4392.82));
		look.push_back(Vector3(7196.82, 1437.07, 4492.82));
	};

	Camera(float pitch, float yaw, Vector3 position){
		this->pitch		= pitch;
		this->yaw		= yaw;
		this->position	= position;
		this->roll		= 0;
	}

	void switchMovement() { freeMove = !freeMove; };
	void switchMiniMap() { miniMap = !miniMap; };

	~Camera(void){};

	void UpdateCamera(float msec = 10.0f);

	//Builds a view matrix for the current camera variables, suitable for sending straight
	//to a vertex shader (i.e it's already an 'inverse camera matrix').
	Matrix4 BuildViewMatrix();

	//Gets position in world space
	Vector3 GetPosition() const { return position;}
	//Sets position in world space
	void	SetPosition(Vector3 val) { position = val;}

	//Gets yaw, in degrees
	float	GetYaw()   const { return yaw;}
	//Sets yaw, in degrees
	void	SetYaw(float y) {yaw = y;}

	//Gets pitch, in degrees
	float	GetPitch() const { return pitch;}
	//Sets pitch, in degrees
	void	SetPitch(float p) {pitch = p;}

	//Gets roll, in degrees
	float	GetRoll() const { return roll; }
	//Sets roll, in degrees
	void	SetRoll(float r) { roll = r; }
	float tim = 0;
	float tim2 = 0;

protected:
	float	yaw;
	float	pitch;
	float	roll;
	Vector3 position;
	Vector3 PrevEnd;
	Vector3 lastPos;

	Vector3 BCurved(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t);

	float alpha = 0.5;
	float tension = 0;

	int point = 0;
	int flPoint = 0;
	float clp2 = 0;

	bool freeMove = true;
	bool miniMap = false;

	int speedDiv = 5;

	std::vector<Vector3> points;
	vector<Vector3> look;
};