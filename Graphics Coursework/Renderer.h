//Author David Towers (160243066)
#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustrum.h"
#include "../nclgl/OBJMesh.h"


#define SHADOWSIZE 2048
#define POST_PASSES 10


class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer();

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void ResetTime() { time = 0; };
	bool loadShaders();

	Light* getLight() { return light; };
	void switchLight() { secondLight = !secondLight; };
	void swapProcess() { postProcess = !postProcess; };

protected:
	void PresentScene();
	void DrawPostProcess();

	void DrawSkyBox();
	void DrawWater();
	void DrawFloor();
	void DrawRain();
	void DrawMesh();
	void DrawBauble();

	void DrawShadowScene();
	void DrawCombinedScene();



	Shader* riverShader;
	Shader* skyboxShader;
	Shader* floorShader;
	Shader* meshShader;
	Shader* shadowShader;
	Shader* rainShader;
	Shader* processShader;
	Shader* presentShader;
	Shader* baubleShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* bufferQuad;
	Mesh* rain;
	Mesh* cube;

	Light* light;
	Light* cLight;
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
	OBJMesh* sphere;

	float waterRotate;
	float time = 0;

	bool postProcess = false;
	bool secondLight = false;
};