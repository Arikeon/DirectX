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
namespace IO
{
	struct TFileIO;
}


class CRenderer
{
	friend class CEngine;
	friend class CD3D11Interface;
	friend struct TMesh;
	friend struct IO::TFileIO;

public:
	void ResetPipeline();
	TShader& GetShader(ShaderID id) { return m_shaders[id]; }
	D3D11_VIEWPORT& GetView(EViews::Type id) { return m_views[id]; }
	CD3D11Interface*& GetD3DInterface() { return m_D3DInterface; }
	TD3DRenderTarget& GetRenderTarget(int index) { return m_rtvs[index]; }
	TD3DDepthTarget& GetDepthTarget(int index) { return m_depthtargets[index]; }
	TD3DBuffer& GetVertexBuffer(BufferID index) { return m_vertexbuffers[index]; }
	TD3DBuffer& GetIndexBuffer(BufferID index) { return m_indexbuffers[index]; }
	TMaterial& GetMaterial(MaterialID index) { return m_materials[index]; }

	TextureID CreateTexture(
		unsigned int width,
		unsigned int height,
		unsigned int depth,
		unsigned int arraySize,
		unsigned int mipLevels,
		DXGI_FORMAT format);
	TD3DTexture& GetTexture(TextureID index) { return m_textures[index]; }
	void DebugCaptureTexture(TD3DTexture texture, bool isFatal = false);

private:
	CRenderer();
	~CRenderer();
	void Initialize(TWindow window);
	void CompileShaders();
	void Restart();
	void Clear();
	void Present();

	std::vector<TShader> m_shaders;
	std::vector<TD3DBuffer> m_vertexbuffers;
	std::vector<TD3DBuffer> m_indexbuffers;
	std::vector<TD3DRenderTarget> m_rtvs;
	std::vector<TD3DTexture> m_textures;
	std::vector<TD3DSampler> m_samplers;
	std::vector<TD3DDepthTarget> m_depthtargets;
	std::vector<TMaterial> m_materials;
	std::array<ID3D11RasterizerState*, ERasterizerStates::eCount> m_rasterizerstates;
	std::array<D3D11_VIEWPORT, EViews::eCount> m_views;

	CD3D11Interface* m_D3DInterface;
};