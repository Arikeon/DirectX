#pragma once
#include "Build.h"
#include "Window.h"
#include "Shader.h"
#include "BufferKeys.h"
#include "BufferStruct.h"
#include "RenderEnums.h"

#include <Windows.h>
#include <vector>

class CD3D11Interface;
struct TMesh;

class CRenderer
{
	friend class CEngine;
	friend class CD3D11Interface;
	friend struct TMesh;

public:
	void ResetPipeline();
	TShader& GetShader(ShaderID id);
	D3D11_VIEWPORT& GetView(EViews::Type id);

private:
	CRenderer();
	~CRenderer();
	void Initialize(TWindow window);
	void CompileShaders();
	void Restart();

	std::vector<TShader> m_shaders;
	std::vector<TD3DBuffer> m_vertexbuffers;
	std::vector<TD3DBuffer> m_indexbuffers;
	std::vector<TD3DRenderTarget> m_rtvs;
	std::vector<TD3DTexture> m_textures;
	std::vector<TD3DSampler> m_samplers;
	std::vector<TD3DDepthTarget> m_depthtargets;
	std::array<ID3D11RasterizerState*, ERasterizerStates::eCount> m_rasterizerstates;
	std::array<D3D11_VIEWPORT, EViews::eCount> m_views;

	CD3D11Interface* m_D3DInterface;
};