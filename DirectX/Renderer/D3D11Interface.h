#pragma once
#include "Window.h"
#include <Window.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgidebug.h>
#include "Shader.h"

class CD3D11Interface
{
	friend struct TMesh;
	friend class CRenderer;
public:
	CD3D11Interface();
	~CD3D11Interface();
	void Present();
	void InitializeD3D(TWindow window);
	void CompileShader(TShader& shader);
	void CreateShaderStage(TShader& shader, EShaderStage::Type stage, void* pshadercode, const size_t shaderbinary);
private:
	ID3D11Device* GetDevice() { return m_device; }
	IDXGISwapChain* m_swapchain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	ID3D11Texture2D* m_backbuffer2D;
	ID3D11Texture2D* m_depthbuffer2D;
	ID3D11RenderTargetView* m_backbufferRTV;
	ID3D11DepthStencilView* m_depthbufferDSV;
	ID3D11RasterizerState* m_rasterizerstate;
#if ENABLE_DXGI_DEBUG
	IDXGIDebug* m_dxgidebug;
#endif
};