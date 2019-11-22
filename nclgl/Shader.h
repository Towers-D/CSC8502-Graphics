/******************************************************************************
Class:Shader
Implements:
Author:Rich Davison	<nrd542@newcastle.ac.uk>
Description:VERY simple class to encapsulate GLSL shader loading, linking,
and binding. Useful additions to this class would be overloaded functions to
replace the glUniformxx functions in external code, and possibly a map to store
uniform names and their resulting bindings. 

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once


#include "OGLRenderer.h"

enum ShaderTypes {
	SHADER_VERTEX		= 0,
	SHADER_FRAGMENT		= 1,
	SHADER_GEOMETRY		= 2,
	SHADER_TESSCONTROL	= 3,
	SHADER_TESSEVAL		= 4,
	SHADER_MAX			= 5
};

using namespace std;
class Shader	{
public:
	Shader(const string& vertex, const string& fragment , const string& geometry = "", const string& control = "", const string& eval = "");
	~Shader(void);

	GLuint  GetProgram() const { return program;}
	
	void	Reload();

	bool	LoadSuccess() const {
		return !loadFailed;
	}

	bool	LinkProgram();
protected:
	void	ClearShaderObjects();
	bool	LoadShaderFile(const string& from, string &into);
	GLuint	GenerateShader(const string& from, GLenum type);
	void	SetDefaultAttributes();
	
	string	shaderNames[SHADER_MAX];
	GLuint	objects[SHADER_MAX];
	GLuint	program;

	bool	loadFailed;
};

