#pragma once
#include "SceneNode.h"
#include "OBJMesh.h"

class CubeRobot : public SceneNode {
public:
	CubeRobot();
	~CubeRobot() {};
	virtual void Update(float msec);

	static  Mesh*  CreateCube() {
		if (!cube) {
		OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR "cube.obj");
		cube = m;
		}
		return cube;
	}

	static void DeleteCube() { delete cube; }

protected:
	static Mesh* cube;
	SceneNode* head;
	SceneNode* leftArm;
	SceneNode* rightArm;
};

