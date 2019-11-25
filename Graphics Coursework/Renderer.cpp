#include "Renderer.h"
#include "../../nclgl/CubeRobot.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera();
	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500.0f, RAW_WIDTH * HEIGHTMAP_X));

	heightMap = new HeightMap(TEXTUREDIR "/terrain.raw");
	quad = Mesh::GenerateQuad();
	rain = Mesh::GenerateQuad();
	test = Mesh::GenerateQuad();
	rain->SetType(GL_PATCHES);
	treeNode = new OBJMesh(MESHDIR "lowtree.obj");

	glViewport(0, 0, 200, 200);
	light =  new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f) * 3, 750, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f) * 3), Vector4(0.9, 0.9, 1, 1), (RAW_WIDTH * HEIGHTMAP_X) * 3);
	loadShaders();

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	red = SOIL_load_OGL_texture(TEXTUREDIR "red.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	green = SOIL_load_OGL_texture(TEXTUREDIR "green.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	blue = SOIL_load_OGL_texture(TEXTUREDIR "blue.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	grass = SOIL_load_OGL_texture(TEXTUREDIR "grass 3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	grassNorm = SOIL_load_OGL_texture(TEXTUREDIR "grass 3Dot3.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	mud = SOIL_load_OGL_texture(TEXTUREDIR "mud.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	mudNorm = SOIL_load_OGL_texture(TEXTUREDIR "mudDot3.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "waterDot3.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "rusted_west.jpg", TEXTUREDIR "rusted_east.jpg", TEXTUREDIR "rusted_up.jpg", TEXTUREDIR "rusted_down.jpg",
									TEXTUREDIR "rusted_south.jpg", TEXTUREDIR "rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	rain->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	treeNode->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	treeNode->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!red || !blue || !green || !grass || !grassNorm || !mud || !mudNorm ||!cubeMap || !quad->GetTexture() || !quad->GetBumpMap() || !heightMap->GetTexture() || !heightMap->GetBumpMap() || !rain->GetTexture() || !treeNode->GetTexture() || !treeNode->GetBumpMap())
		return;


	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	SetTextureRepeating(grass, true);
	SetTextureRepeating(grassNorm, true);
	SetTextureRepeating(mud, true);
	SetTextureRepeating(mudNorm, true);

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(quad->GetBumpMap(), true);

	SetTextureRepeating(rain->GetTexture(), true);

	SetTextureRepeating(treeNode->GetTexture(), true);
	SetTextureRepeating(treeNode->GetBumpMap(), true);

	//SHADOW MAP
	glGenTextures(1, &ShadowTex);
	glBindTexture(GL_TEXTURE_2D, ShadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &ShadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ShadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ShadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Generate Dpeth Textures for Post Processing
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &processFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0])
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	waterRotate = 0.0f;
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	glScissor(width - 200, height - 200, 200, 200);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}

Renderer::~Renderer() {
	delete camera;
	delete heightMap;
	delete quad;
	delete rain;

	delete light;

	delete reflectShader;
	delete skyboxShader;
	delete riverShader;
	delete shadowShader;
	delete sceneShader;
	delete treeNode;
	delete rainShader;
	delete processShader;
	delete presentShader;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &ShadowTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
	glDeleteFramebuffers(1, &ShadowFBO);

	currentShader = 0;
}

bool Renderer::loadShaders() {
	reflectShader = new Shader(SHADERDIR "PerPixelVertex.glsl", SHADERDIR "reflectFragment.glsl");
	if (!reflectShader->LinkProgram())
		return false;

	skyboxShader = new Shader(SHADERDIR "skyboxVertex.glsl", SHADERDIR "skyboxFragment.glsl");
	if (!skyboxShader->LinkProgram())
		return false;

	riverShader = new Shader(SHADERDIR "riverVert.glsl", SHADERDIR "riverFrag.glsl");
	if (!riverShader->LinkProgram())
		return false;

	sceneShader = new Shader(SHADERDIR "shadowscenevert.glsl", SHADERDIR "shadowscenefrag.glsl");
	if (!sceneShader->LinkProgram())
		return false;

	shadowShader = new Shader(SHADERDIR "shadowVert.glsl", SHADERDIR "shadowFrag.glsl");
	if (!shadowShader->LinkProgram())
		return false;

	rainShader = new Shader(SHADERDIR  "simpleVert.glsl", SHADERDIR "simpleFrag.glsl", SHADERDIR "rainGeom.glsl", SHADERDIR "rainTCS.glsl", SHADERDIR "rainTES.glsl");
	if (!rainShader->LinkProgram())
		return false;

	processShader = new Shader(SHADERDIR  "TexturedVertex.glsl", SHADERDIR "processFrag.glsl");
	if (!processShader->LinkProgram())
		return false;
		
	presentShader = new Shader(SHADERDIR  "TexturedVertex.glsl", SHADERDIR "TexturedFragment.glsl");
	if (!presentShader->LinkProgram())
		return false;

	return true;
 }

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	waterRotate += msec / 1000.0f;
	time += msec;
}

void Renderer::RenderScene() {
	Vector3 currPos = camera->GetPosition();
	float currRoll = camera->GetRoll();
	float currYaw = camera->GetYaw();
	float currPitch = camera->GetPitch();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, width, height);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	
	DrawShadowScene();
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DrawSkyBox();
	DrawCombinedScene();
	DrawRain();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (postProcess)
		DrawPostProcess();
	PresentScene();
	

	camera->SetPosition(Vector3((RAW_WIDTH * HEIGHTMAP_X / 2.0f) * 3, 12000, (RAW_WIDTH * HEIGHTMAP_Z / 2.0f) * 3));
	camera->SetPitch(-90);
	camera->SetYaw(0);
	camera->SetRoll(0);
	
	glViewport(width - 200, height - 200, 200, 200);
	glEnable(GL_SCISSOR_TEST);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)(width - 200) / (float)(height - 200), 35);
	DrawCombinedScene();
	camera->SetPosition(currPos);
	camera->SetPitch(currPitch);
	camera->SetYaw(currYaw);
	camera->SetRoll(currRoll);

	SwapBuffers();
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, ShadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	SetCurrentShader(shadowShader);
	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	textureMatrix = biasMatrix * (projMatrix * viewMatrix);
	UpdateShaderMatrices();
	DrawWater();
	DrawFloor();
	DrawMesh();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	viewMatrix = camera->BuildViewMatrix();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene() {
	
	
	SetCurrentShader(reflectShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 3);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	SetShaderLight(*light);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ShadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
	DrawWater();

	SetCurrentShader(riverShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 3);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	SetShaderLight(*light);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ShadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
	DrawFloor();

	/*SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 3);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	SetShaderLight(*light);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ShadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	DrawMesh();*/
	glUseProgram(0);
}

void Renderer::DrawSkyBox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawWater() {
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f) * 3;
	float heightY = 256 * HEIGHTMAP_Y / 6.0f;
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f) * 3;

	modelMatrix = Matrix4::Translation(Vector3(heightX, heightY - 100, heightZ)) * Matrix4::Scale(Vector3(heightX, 1, heightZ)) * Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
	//Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&textureMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);
	quad->Draw();
}

void Renderer::DrawFloor() {
	float heightY = 256 * HEIGHTMAP_Y / 3.0f + 1000.0f;

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "grass"), 6);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "grassNorm"), 8);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "mud"), 7);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "mudNorm"), 9);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "red"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "green"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "blue"), 5);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, red);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, green);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, blue);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, grass);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, grassNorm);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, mud);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, mudNorm);

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "defy"), heightY);

	modelMatrix = Matrix4::Scale(Vector3(3, 3, 3));
	textureMatrix.ToIdentity();
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	heightMap->Draw();
}

void Renderer::DrawRain() {
	SetCurrentShader(rainShader);
	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f) * 3;
	float heightY = 5000 + 256 * HEIGHTMAP_Y / 6.0f;
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f) * 3;

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "tli"), 2048);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "tlo"), 2048);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "defy"), heightY);

	modelMatrix = Matrix4::Translation(Vector3(heightX, heightY, heightZ)) * Matrix4::Scale(Vector3(heightX, 1, heightZ)) * Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	UpdateShaderMatrices();
	rain->Draw();
	glUseProgram(0);
}

void Renderer::DrawMesh() {
	modelMatrix = Matrix4::Scale(Vector3(200, 200, 200)) * Matrix4::Translation(Vector3(11, 1, 4));
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);
	treeNode->Draw();
}

void Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(processShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	for (int i = 0; i < POST_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		test->SetTexture(bufferColourTex[0]);
		test->Draw();

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

		test->SetTexture(bufferColourTex[1]);
		test->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(presentShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
	test->SetTexture(bufferColourTex[0]);
	test->Draw();
	glUseProgram(0);
}