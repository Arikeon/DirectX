#pragma once
#include "Build.h"
#include "Window.h"
#include "Shader.h"
#include "BufferKeys.h"
#include "BufferStruct.h"
#include "RenderEnums.h"
#include "FrameBuffer.h"
#include "Camera.h"

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
	void ConstructFrameBuffer(TWindow window, CCamera camera);
	void UnbindRTV();
	void UnbindSRV(TShader shader);
	TShader& GetShader(ShaderID id) { return m_shaders[id]; }
	D3D11_VIEWPORT& GetView(EViews::Type id) { return m_views[id]; }
	CD3D11Interface*& GetD3DInterface() { return m_D3DInterface; }
	TD3DRenderTarget& GetRenderTarget(int index) { return m_rtvs[index]; }
	TD3DDepthTarget& GetDepthTarget(int index) { return m_depthtargets[index]; }
	TD3DBuffer& GetVertexBuffer(BufferID index) { return m_vertexbuffers[index]; }
	TD3DBuffer& GetIndexBuffer(BufferID index) { return m_indexbuffers[index]; }
	TD3DBuffer& GetInstanceBuffer(BufferID index) { return m_instancebuffers[index]; }
	TMaterial& GetMaterial(MaterialID index) { return m_materials[index]; }
	std::vector<TMaterial>& GetMaterials() { return m_materials; }
	TD3DSampler& GetSampler(SamplerID index) { return m_samplers[index]; }
	TD3DRenderTarget& GetGBufferRTV(int32 index) { return m_GBuffer.GetRTV(index); }
	TD3DDepthTarget& GetGBufferDepth() { return m_GBuffer.m_GBufferDepth; }
	ID3D11RasterizerState* GetRenderState(int32 index) { return m_rasterizerstates[index]; }
	std::vector<struct ID3D11RenderTargetView*> GetGBuffer();
	FrameBuffer& GetFrameBuffer() { check(m_frameBuffer.IsUpToDate());  return m_frameBuffer; }
	TD3DDepthStencilState& GetDepthStencilState(int32 index) { check(m_depthstencilstates[index].IsValid()); return m_depthstencilstates[index]; }

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

	FrameBuffer m_frameBuffer;

	std::vector<TShader> m_shaders;
	std::vector<TD3DBuffer> m_vertexbuffers;
	std::vector<TD3DBuffer> m_indexbuffers;
	std::vector<TD3DBuffer> m_instancebuffers;
	std::vector<TD3DRenderTarget> m_rtvs;
	std::vector<TD3DTexture> m_textures;
	std::vector<TD3DSampler> m_samplers;
	std::vector<TD3DDepthTarget> m_depthtargets;
	std::vector<TD3DDepthStencilState> m_depthstencilstates;
	std::vector<TMaterial> m_materials;
	std::array<ID3D11RasterizerState*, ERasterizerStates::eCount> m_rasterizerstates;
	std::array<D3D11_VIEWPORT, EViews::eCount> m_views;

	TGBuffer m_GBuffer = {};
	CD3D11Interface* m_D3DInterface;
};