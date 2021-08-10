#pragma once
#include "Window.h"
#include "BufferStruct.h"
#include "BufferKeys.h"
#include "Shader.h"
#include "RenderEnums.h"
#include "D3DDefines.h"

#include <d3d11.h>
#include <dxgi.h>
#include <dxgidebug.h>
#include <Windows.h>

class CRenderer;

class CD3D11Interface
{
	static CRenderer* prenderer;
	friend struct TMesh;
	friend class CRenderer;
public:
	CD3D11Interface();
	~CD3D11Interface();
	void ClearBackBuffer();
	void ClearGBuffer(TGBuffer& gbuffer);
	void Present();
	void InitializeD3D(TWindow window);
	void CompileShader(TShader& shader);
	void CreateShaderStage(TShader& shader, EShaderStage::Type stage, void* pshadercode, const size_t shaderbinary);
	ID3D11DeviceContext* GetContext() { return m_context; }
	ID3D11Device* GetDevice() { return m_device; }

	//Helpers
	TD3DSampler CreateSampler(
		D3D11_TEXTURE_ADDRESS_MODE aU,
		D3D11_TEXTURE_ADDRESS_MODE aV,
		D3D11_TEXTURE_ADDRESS_MODE aW,
		D3D11_COMPARISON_FUNC comparisonFunc,
		std::string debugname = ""
	);

	TD3DTexture CreateTexture(
		unsigned int width,
		unsigned int height,
		unsigned int depth,
		unsigned int arraySize,
		unsigned int mipLevels,
		DXGI_FORMAT format);

	//Render Commands
	void UnbindRTV();
	void UnbindSRV(TShader shader);
	void ResizeViewPorts(TWindow window);

	float m_nearplane = 0.00001f, m_farplane = 1.0f;

private:
	IDXGISwapChain* m_swapchain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
#if ENABLE_DXGI_DEBUG
	IDXGIDebug* m_dxgidebug;
#endif
};