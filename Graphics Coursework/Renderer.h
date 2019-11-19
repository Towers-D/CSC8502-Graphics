#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustrum.h"
#include "../../nclgl/OBJMesh.h"


#define SHADOWSIZE 2048


class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer();

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void ResetTime() { time = 0; };
	void loadShaders();

	Light* getLight() { return light; }

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkyBox();
	void DrawFloor();

	void DrawMesh();
	void DrawShadowScene();
	void DrawCombinedScene();

	SceneNode* root;
	Frustrum frameFrustrum;

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* riverShader;
	Shader* sceneShader;
	Shader* shadowShader;

	HeightMap* heightMap;
	Mesh* quad;

	Light* light;
	Camera* camera;

	GLuint cubeMap;
	GLuint red;
	GLuint green;
	GLuint blue;

	GLuint ShadowTex;
	GLuint ShadowFBO;

	OBJMesh* treeNode;

	float waterRotate;
	float time = 0;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};