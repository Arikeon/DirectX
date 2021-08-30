#pragma once
#include "Build.h"
#include "Window.h"
#include "Shader.h"
#include "BufferKeys.h"
#include "BufferStruct.h"
#include "RenderEnums.h"

#include <Windows.h>
#include <vector>

#define RTVSLOTMAX 8

namespace EStaticSamplerKey
{
	enum ID
	{
		eDefault = 0,
	};
}

namespace ERenderTargetKey
{
	enum ID
	{
		eBackBufeer		= 0,
		eMax
	};
}

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
	void UnbindRTV();
	void UnbindSRV(TShader shader);
	TShader& GetShader(ShaderID id) { return m_shaders[id]; }
	D3D11_VIEWPORT& GetView(EViews::Type id) { return m_views[id]; }
	CD3D11Interface*& GetD3DInterface() { return m_D3DInterface; }
	TD3DRenderTarget& GetRenderTarget(int index) { return m_rtvs[index]; }
	TD3DDepthTarget& GetDepthTarget(int index) { return m_depthtargets[index]; }
	TD3DBuffer& GetVertexBuffer(BufferID index) { return m_vertexbuffers[index]; }
	TD3DBuffer& GetIndexBuffer(BufferID index) { return m_indexbuffers[index]; }
	TMaterial& GetMaterial(MaterialID index) { return m_materials[index]; }
	TD3DSampler& GetSampler(SamplerID index) { return m_samplers[index]; }
	TD3DRenderTarget& GetGBufferRTV(int32 index) { return m_GBuffer.GetRTV(index); }
	ID3D11RasterizerState* GetRenderState(int32 index) { return m_rasterizerstates[index]; }
	std::vector<struct ID3D11RenderTargetView*> GetGBuffer();

	TextureID CreateTexture(
		unsigned int width,
		unsigned int height,
		unsigned int depth,
		unsigned int arraySize,
		unsigned int mipLevels,
		DXGI_FORMAT format);

	TextureID CreateTexture(struct ID3D11Resource* resource, struct ID3D11ShaderResourceView* srv);

	TD3DTexture& GetTexture(TextureID index) { return m_textures[index]; }
	void DebugCaptureTexture(TD3DTexture texture, bool isFatal = false);

	TextureID DefaultTexture2D = -1;
	TextureID DefaultTexture3D = -1;

private:
	CRenderer();
	~CRenderer();
	void Initialize(TWindow window);
	void CreateGBufferRenderTargets(TWindow window, bool bResize = false);
	void CreateStaticTextures();
	void CreateStaticSamplers();
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

	TGBuffer m_GBuffer = {};
	CD3D11Interface* m_D3DInterface;
};