#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustrum.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"

#define SHADOWSIZE 2048


class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer();

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void ResetTime() { time = 0; };
	void loadShaders();

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkyBox();
	void DrawFloor();

	void DrawMesh();
	void DrawCombined();

	SceneNode* root;
	Frustrum frameFrustrum;

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* riverShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* test;

	Light* light;
	Camera* camera;

	GLuint cubeMap;
	GLuint moveTex;

	MD5FileData* treeData;
	MD5Node* treeNode;

	float waterRotate;
	float time = 0;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};