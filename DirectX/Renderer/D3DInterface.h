#pragma once
#include "Build.h"
#include "Window.h"
#include "Shader.h"

class CD3DInterface
{
public:
	virtual ~CD3DInterface() {};
	virtual void Present() = 0;
	virtual void InitializeD3D(TWindow window) = 0;
	virtual void CompileShader(TShader& shader) = 0;
};