#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/MD5Mesh.h"
#include "../nclgl/MD5Node.h"

#define SHADOWSIZE 2048

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer();

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected:
	void DrawMesh();
	void DrawFloor();
	void DrawShadowScene();
	void DrawCombinedScene();

	Shader* meshShader;
	Shader* shadowShader;

	GLuint shadowTex;
	GLuint shadowFBO;

	MD5FileData* hellData;
	MD5Node* hellNode;
	Mesh* floor;
	Camera* camera;
	Light* light;
};