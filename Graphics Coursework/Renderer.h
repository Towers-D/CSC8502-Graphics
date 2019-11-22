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
	bool loadShaders();

	Light* getLight() { return light; };

protected:
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

	HeightMap* heightMap;
	Mesh* quad;
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

	OBJMesh* treeNode;

	float waterRotate;
	float time = 0;
};