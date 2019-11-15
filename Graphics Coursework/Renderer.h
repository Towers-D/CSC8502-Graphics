#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer();

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkyBox();
	void DrawFloor();

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

	float waterRotate;
};