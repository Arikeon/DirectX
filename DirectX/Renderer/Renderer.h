#pragma once
#include "Build.h"
#include "Window.h"
#include "Shader.h"
#include "BufferKeys.h"
#include "BufferStruct.h"
#include <Windows.h>
#include <vector>

class CD3D11Interface;
struct TMesh;

class CRenderer
{
	friend class CEngine;
	friend class CD3D11Interface;
	friend struct TMesh;
private:
	CRenderer();
	~CRenderer();
	void Initialize(TWindow window);
	void CompileShaders();
	void Render(float delta);
	void Restart();

	std::vector<TShader> m_shaders;
	std::vector<TD3DBuffer> m_vertexbuffers;
	std::vector<TD3DBuffer> m_indexbuffers;

	CD3D11Interface* m_D3DInterface;
};