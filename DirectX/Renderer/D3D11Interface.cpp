#include "D3D11Interface.h"

CD3D11Interface::CD3D11Interface() :
	CD3DInterface(),
	m_swapchain(nullptr),
	m_device(nullptr),
	m_context(nullptr),
	m_backbuffer2D(nullptr),
	m_depthbuffer2D(nullptr),
	m_backbufferRTV(nullptr),
	m_depthbufferDSV(nullptr),
	m_rasterizerstate(nullptr),
#if ENABLE_DXGI_DEBUG
	m_dxgidebug(nullptr)
#endif
{
}

CD3D11Interface::~CD3D11Interface()
{
	m_swapchain->Release();
	m_device->Release();
	m_context->Release();
	m_backbuffer2D->Release();
	m_depthbuffer2D->Release();
	m_backbufferRTV->Release();
	m_depthbufferDSV->Release();
	m_rasterizerstate->Release();
#if ENABLE_DXGI_DEBUG
	m_dxgidebug->Release();
#endif
}

void CD3D11Interface::InitializeD3D(TWindow window)
{
	bool bSuccess = true;
	HRESULT r;

	//Swap Chain
	{
		DXGI_MODE_DESC ModeDesc = {};
		ModeDesc.Width = (UINT)window.m_width;
		ModeDesc.Height = (UINT)window.m_height;
		ModeDesc.RefreshRate = { 60, 1 };
		ModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		ModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		ModeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
		SwapChainDesc.BufferDesc = ModeDesc;
		SwapChainDesc.SampleDesc = { 1, 0 };
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 1;
		SwapChainDesc.OutputWindow = window.m_hWND;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.Windowed = !window.m_isFullScreen;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		D3D_FEATURE_LEVEL FeatureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};

		D3D_FEATURE_LEVEL OutputFeatureLevel;

		D3D11_CREATE_DEVICE_FLAG DeviceFlags = D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_SINGLETHREADED;
		r = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
#if ENABLE_DXGI_DEBUG
			D3D11_CREATE_DEVICE_DEBUG,
#else
			NULL,
#endif
			FeatureLevels,
			1,
			D3D11_SDK_VERSION,
			&SwapChainDesc,
			& m_swapchain,
			& m_device,
			&OutputFeatureLevel,
			& m_context);

		checkhr(r);

		check(m_swapchain
			&& m_device
			&& m_context);

#if ENABLE_DXGI_DEBUG
		r = m_device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&m_dxgidebug));
		checkhr(r);
		check(m_dxgidebug);
#endif
	}

	//Initialize backbuffer ref
	{
		m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_backbuffer2D);
		r = m_device->CreateRenderTargetView(m_backbuffer2D, nullptr, &m_backbufferRTV);
		checkhr(r);
	}

	//Initialize zbuffer ref
	{
		//Depth Buffer
		D3D11_TEXTURE2D_DESC DepthTextureDesc = {};
		DepthTextureDesc.Height = window.m_height;
		DepthTextureDesc.Width = window.m_width;
		DepthTextureDesc.MipLevels = 1;
		DepthTextureDesc.ArraySize = 1;
		DepthTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
		DepthTextureDesc.SampleDesc.Count = 1;
		DepthTextureDesc.SampleDesc.Quality = 0;
		DepthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		DepthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		DepthTextureDesc.CPUAccessFlags = false;

		r = m_device->CreateTexture2D(&DepthTextureDesc, 0, &m_depthbuffer2D);
		checkhr(r);

		D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
		DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

		r = m_device->CreateDepthStencilView(m_depthbuffer2D, &DepthStencilViewDesc, &m_depthbufferDSV);
		checkhr(r);

		D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
		DepthStencilDesc.DepthEnable = true;
		DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		check(m_backbuffer2D
			&& m_backbufferRTV
			&& m_depthbuffer2D
			&& m_depthbufferDSV);
	}

	//Initialize rasterizer state
	{
		D3D11_RASTERIZER_DESC RasterizerDesc = {};
		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.CullMode = D3D11_CULL_BACK;
		RasterizerDesc.DepthClipEnable = true;
		RasterizerDesc.FrontCounterClockwise = true;
		RasterizerDesc.DepthBias = false;
		RasterizerDesc.DepthBiasClamp = 0;
		RasterizerDesc.SlopeScaledDepthBias = 0;
		RasterizerDesc.ScissorEnable = false;
		RasterizerDesc.MultisampleEnable = false;
		RasterizerDesc.AntialiasedLineEnable = false;

		r = m_device->CreateRasterizerState(&RasterizerDesc, &m_rasterizerstate);
		checkhr(r);

		check(m_rasterizerstate);
	}

	if (bSuccess == false)
	{
		CONSOLE_LOG("Failed to initialized d3d");
		exit(1);
	}
}

void CD3D11Interface::CompileShader(TShader& shader)
{
	int NumberOfStages = 0;

	for (int stage = 0; stage < EShaderStage::eCount; ++stage)
	{
		if (shader.m_shaderstages[(EShaderStage::Type)stage] == false)
			continue;
		NumberOfStages++;

		std::string path = shader.GetDirectoryFromStage((EShaderStage::Type)stage);

		std::string errmsg;
		ID3D10Blob* pblob;


	}

	if (NumberOfStages == 0)
		check(0);
}

void CD3D11Interface::CreateShaderStage(TShader shader, EShaderStage::Type stage, void* pshadercode, const size_t shaderbinary)
{
	HRESULT r;

	switch (stage)
	{
		case EShaderStage::eVS:
		{
			r = m_device->CreateVertexShader(pshadercode, shaderbinary, NULL, &shader.m_compiledstages.m_vs);
			checkhr(r);
		}
			break;
		case EShaderStage::eHS:
		{
			r = m_device->CreateHullShader(pshadercode, shaderbinary, NULL, &shader.m_compiledstages.m_hs);
			checkhr(r);
		}
			break;
		case EShaderStage::eDS:
		{
			r = m_device->CreateDomainShader(pshadercode, shaderbinary, NULL, &shader.m_compiledstages.m_ds);
			checkhr(r);
		}
			break;
		case EShaderStage::eGS:
		{
			r = m_device->CreateGeometryShader(pshadercode, shaderbinary, NULL, &shader.m_compiledstages.m_gs);
			checkhr(r);
		}
			break;
		case EShaderStage::ePS:
		{
			r = m_device->CreatePixelShader(pshadercode, shaderbinary, NULL, &shader.m_compiledstages.m_ps);
			checkhr(r);
		}
			break;
		case EShaderStage::eCS:
		{
			r = m_device->CreateComputeShader(pshadercode, shaderbinary, NULL, &shader.m_compiledstages.m_cs);
			checkhr(r);
		}
		break;
	}
}