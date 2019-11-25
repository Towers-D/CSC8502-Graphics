#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustrum.h"
#include "../../nclgl/OBJMesh.h"


#define SHADOWSIZE 2048
#define POST_PASSES 10


class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer();

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void ResetTime() { time = 12000; };
	bool loadShaders();

	Light* getLight() { return light; };
	void swapProcess() { postProcess = !postProcess; };

protected:
	void PresentScene();
	void DrawPostProcess();

	void DrawSkyBox();
	void DrawWater();
	void DrawFloor();
	void DrawRain();
	void DrawMesh();

	void DrawShadowScene();
	void DrawCombinedScene();


	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* riverShader;
	Shader* sceneShader;
	Shader* shadowShader;
	Shader* rainShader;
	Shader* processShader;
	Shader* presentShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* test;
	Mesh* rain;

	Light* light;
	Light* nolight;
	Camera* camera;
	Camera* overhead;

	GLuint cubeMap;
	GLuint red;
	GLuint green;
	GLuint blue;
	GLuint grass;
	GLuint grassNorm;
	GLuint mud;
	GLuint mudNorm;

	GLuint ShadowTex;
	GLuint ShadowFBO;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;

	OBJMesh* treeNode;

	float waterRotate;
	float time = 0;

	bool postProcess = false;
};