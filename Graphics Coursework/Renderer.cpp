#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	size = parent.GetScreenSize();
	camera = new Camera();
	heightMap = new HeightMap(TEXTUREDIR "/terrain.raw");
	quad = Mesh::GenerateQuad();
	rain = Mesh::GenerateQuad();
	rain->SetType(GL_PATCHES);

	treeNode = new OBJMesh(MESHDIR "lowtree.obj");
	treeNode->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	treeNode->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	glViewport(0, 0, 200, 200);
	
	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500.0f, RAW_WIDTH * HEIGHTMAP_X));
	
	

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f) * 3, 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f) * 3), Vector4(0.9f, 0.9f, 1.0f, 1), (RAW_WIDTH * HEIGHTMAP_X) * 3);

	loadShaders();
	if (!reflectShader->LinkProgram() || !rainShader->LinkProgram() || !skyboxShader->LinkProgram() || !treeShader->LinkProgram() || !riverShader->LinkProgram() || !shadowShader->LinkProgram() || !sceneShader->LinkProgram())
		return;


	red = SOIL_load_OGL_texture(TEXTUREDIR "red.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	green = SOIL_load_OGL_texture(TEXTUREDIR "green.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	blue = SOIL_load_OGL_texture(TEXTUREDIR "blue.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	grass = SOIL_load_OGL_texture(TEXTUREDIR "grass 3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	mud = SOIL_load_OGL_texture(TEXTUREDIR "mud.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	


	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "waterDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	rain->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	rain->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "waterDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "rusted_west.jpg", TEXTUREDIR "rusted_east.jpg",
									TEXTUREDIR "rusted_up.jpg", TEXTUREDIR "rusted_down.jpg",
									TEXTUREDIR "rusted_south.jpg", TEXTUREDIR "rusted_north.jpg",
									SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

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

	if (!cubeMap || !quad->GetTexture() || quad->GetBumpMap() || !heightMap->GetTexture() || !heightMap->GetBumpMap())
		return;


	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(quad->GetBumpMap(), true);
	SetTextureRepeating(rain->GetTexture(), true);
	SetTextureRepeating(rain->GetBumpMap(), true);

	SetTextureRepeating(treeNode->GetTexture(), true);
	SetTextureRepeating(treeNode->GetBumpMap(), true);

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	SetTextureRepeating(grass, true);
	SetTextureRepeating(mud, true);

	init = true;
	waterRotate = 0.0f;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glScissor(size.x - 200, size.y - 200, 200, 200);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

Renderer::~Renderer() {
	delete camera;
	delete heightMap;
	delete quad;
	delete rain;
	delete reflectShader;
	delete skyboxShader;
	//delete lightShader;
	delete riverShader;
	delete shadowShader;
	delete sceneShader;
	delete treeNode;
	delete rainShader;
	delete light;
	currentShader = 0;
}

void Renderer::loadShaders() {
	reflectShader = new Shader(SHADERDIR "PerPixelVertex.glsl", SHADERDIR "reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR "skyboxVertex.glsl", SHADERDIR "skyboxFragment.glsl");
	//lightShader = new Shader(SHADERDIR "PerPixelVertex.glsl", SHADERDIR "BumpFragment.glsl");

	riverShader = new Shader(SHADERDIR "riverVert.glsl", SHADERDIR "riverFrag.glsl");
	sceneShader = new Shader(SHADERDIR "shadowscenevert.glsl", SHADERDIR "shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR "shadowVert.glsl", SHADERDIR "shadowFrag.glsl");
	treeShader = new Shader(SHADERDIR "shadowVer2.glsl", SHADERDIR "shadowFrag2.glsl");
	rainShader = new Shader(SHADERDIR  "TexturedVertex.glsl", SHADERDIR "TexturedFragment.glsl", SHADERDIR "rainGeom.glsl", SHADERDIR "rainTCS.glsl", SHADERDIR "rainTES.glsl");
 }

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	/*viewMatrix = camera->BuildViewMatrix();*/
	waterRotate += msec / 1000.0f;
	time += msec;
}

void Renderer::RenderScene() {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	Vector4 store = Vector4(0, 0, width, height);
	glViewport(0, 0, width, height);
	
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	DrawSkyBox();
	DrawShadowScene(store);
	DrawFloor();
	DrawWater();
	DrawCombinedScene();

	DrawRain();
	


	Vector3 currPos = camera->GetPosition();
	float currRoll = camera->GetRoll();
	float currYaw = camera->GetYaw();
	float currPitch = camera->GetPitch();

	
	
	camera->SetPosition(Vector3((RAW_WIDTH * HEIGHTMAP_X / 2.0f) * 3, 12000, (RAW_WIDTH * HEIGHTMAP_Z / 2.0f) * 3));
	camera->SetPitch(-90);
	camera->SetYaw(0);
	camera->SetRoll(0);
	
	glViewport(size.x - 200, size.y - 200, 200, 200);
	glEnable(GL_SCISSOR_TEST);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)(size.x - 200) / (float)(size.y - 200), 35);
	DrawSkyBox();
	DrawWater();
	DrawFloor();
	DrawCombinedScene();
	DrawRain();

	

	SwapBuffers();


	camera->SetPosition(currPos);
	camera->SetPitch(currPitch);
	camera->SetYaw(currYaw);
	camera->SetRoll(currRoll);

}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 3);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ShadowTex);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f) * 3;
	float heightY = 256 * HEIGHTMAP_Y / 6.0f;
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f) * 3;

	modelMatrix = Matrix4::Translation(Vector3(heightX, heightY - 100, heightZ)) * Matrix4::Scale(Vector3(heightX, 1, heightZ)) * Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();
	quad->Draw();
	glUseProgram(0);
}

void Renderer::DrawWaterS() {
	UpdateShaderMatrices();
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);
	quad->Draw();
}

void Renderer::DrawHeightmap() {
	UpdateShaderMatrices();
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

void Renderer::DrawShadowScene(Vector4 store) {
	glBindFramebuffer(GL_FRAMEBUFFER, ShadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	SetCurrentShader(shadowShader);
	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	textureMatrix = biasMatrix * (projMatrix * viewMatrix);
	UpdateShaderMatrices();
	DrawWaterS();
	DrawHeightmap();
	DrawMesh();

	

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(store.x, store.y, store.z, store.w);
	viewMatrix = camera->BuildViewMatrix();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 3);

	SetShaderLight(*light);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ShadowTex);

	//viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
	DrawMesh();
	glUseProgram(0);
}

void Renderer::DrawMesh() {
	modelMatrix = Matrix4::Scale(Vector3(200, 200, 200)) * Matrix4::Translation(Vector3(11, 1, 4));
	//UpdateShaderMatrices();
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	treeNode->Draw();
}

void Renderer::DrawSkyBox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}



void Renderer::DrawFloor() {
	SetCurrentShader(riverShader);

	//float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);
	float heightY = 256 * HEIGHTMAP_Y / 3.0f + 1000.0f;
	//float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);
	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "grass"), 6);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "mud"), 7);

	SetShaderLight(*light);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "red"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "green"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "blue"), 5);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ShadowTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, red);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, green);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, blue);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, grass);
		glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, mud);

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "defy"), heightY);

	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Scale(Vector3(3, 3, 3));
	textureMatrix.ToIdentity();
	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	//viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	
	
	heightMap->Draw();
	glUseProgram(0);
}