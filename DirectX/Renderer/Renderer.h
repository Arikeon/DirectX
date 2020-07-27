#pragma once
#include "Build.h"
#include "Window.h"
#include "Shader.h"
#include <Windows.h>
#include <vector>

class CD3DInterface;

class CRenderer
{
	friend class CEngine;
private:
	CRenderer();
	~CRenderer();
	void Initialize(TWindow window);
	void CompileShaders();
	void Update(float delta);

	std::vector<TShader> m_shaders;
	CD3DInterface* m_D3DInterface;
};