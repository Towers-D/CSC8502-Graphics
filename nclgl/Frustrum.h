#pragma once

#include "Plane.h"
#include "Matrix4.h"
#include "SceneNode.h"

class Matrix4; //compile Matrix 4 before this class

class Frustrum {
public:
	Frustrum() {};
	~Frustrum() {};
	void FromMatrix(const Matrix4 &mvp);
	bool InsideFrustrum(SceneNode& n);

protected:
	Plane planes[6];
};