#pragma once
#include "Window.h"
#include "BufferKeys.h"
#include "Shader.h"
#include "RenderEnums.h"

#include <d3d11.h>
#include <dxgi.h>
#include <dxgidebug.h>
#include <Window.h>

class CRenderer;

class CD3D11Interface
{
	static CRenderer* prenderer;
	friend struct TMesh;
	friend class CRenderer;
public:
	CD3D11Interface();
	~CD3D11Interface();
	void Present();
	void InitializeD3D(TWindow window);
	void CompileShader(TShader& shader);
	void CreateShaderStage(TShader& shader, EShaderStage::Type stage, void* pshadercode, const size_t shaderbinary);

	//Render Commands
	void UnbindTargets();
	void ResizeViewPorts(TWindow window);

	float m_nearplane = 0.1f, m_farplane = 1.0000f;

private:
	ID3D11Device* GetDevice() { return m_device; }
	IDXGISwapChain* m_swapchain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
#if ENABLE_DXGI_DEBUG
	IDXGIDebug* m_dxgidebug;
#endif
};