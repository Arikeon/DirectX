#include "D3D11Interface.h"
#include "HLSLGlue.h"
#include <d3dcompiler.h>

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

void CD3D11Interface::Present()
{

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
#if ENABLE_DEBUG
	const UINT SHADER_COMPILE_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	const UINT SHADER_COMPILE_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

	int NumberOfStages = 0;

	HRESULT r;
	D3DBlobs Blobs;

	for (int i = 0; i < EShaderStage::eCount; ++i)
	{
		EShaderStage::Type stage = (EShaderStage::Type)i;

		if (shader.m_usedshaderstages[(EShaderStage::Type)stage] == false)
			continue;
		NumberOfStages++;

		std::string pathstr = shader.GetDirectoryFromStage((EShaderStage::Type)stage);
		std::wstring pathwstr = Algorithm::string_to_wstring(pathstr);

		ID3DBlob* errmsg, *pblob;
		std::vector<D3D10_SHADER_MACRO> NullTerminatedMacro = shader.m_shadermacros;
		NullTerminatedMacro.push_back({ NULL, NULL });

		r = D3DCompileFromFile(
			pathwstr.c_str(),
			NullTerminatedMacro.data(),
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			TShader::GetEntryPointName(stage).c_str(),
			TShader::GetFeatureLevel(stage).c_str(),
			SHADER_COMPILE_FLAGS,
			NULL,
			&pblob,
			&errmsg);

		if (r != S_OK || pblob == nullptr)
		{
			char* msg = (char*)errmsg->GetBufferPointer();
			size_t msgsize = errmsg->GetBufferSize();

			std::stringstream strs;
			for (int i = 0; i < msgsize; ++i)
				strs << msg;

			volatile std::string errmsgstr = strs.str();

			if (pblob)
				pblob->Release();
			errmsg->Release();

			check(0);
		}

		Blobs.m_stageblob[stage] = pblob;

		CreateShaderStage(shader, stage, Blobs.m_stageblob[stage]->GetBufferPointer(), Blobs.m_stageblob[stage]->GetBufferSize());


		//set reflections
		{
			for (unsigned stage = EShaderStage::eVS; stage < EShaderStage::eCount; ++stage)
			{
				if (Blobs.m_stageblob[stage] != nullptr)
				{
					void** pbuffer = reinterpret_cast<void**>(&shader.m_shaderreflections.m_stagereflection[stage]);
					r = D3DReflect(Blobs.m_stageblob[stage]->GetBufferPointer(), Blobs.m_stageblob[stage]->GetBufferSize(), IID_ID3D11ShaderReflection, pbuffer);
					checkhr(r);
				}
			}
		}
	}

	//Atleast one shader have to compile
	check(NumberOfStages > 0);

	//Compile InputLayout if it uses vertex shader
	if (shader.m_usedshaderstages[EShaderStage::eVS] == true)
	{
		ID3D11ShaderReflection* VSReflection = shader.m_shaderreflections.m_vsreflection;

		D3D11_SHADER_DESC ShaderDesc = {};
		VSReflection->GetDesc(&ShaderDesc);
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout(ShaderDesc.InputParameters);
		D3D_PRIMITIVE primitiveDesc = ShaderDesc.InputPrimitive;

		shader.m_info.m_instructioncount = ShaderDesc.InstructionCount;

		for (int i = 0; i < (int)ShaderDesc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC ParamDesc;
			VSReflection->GetInputParameterDesc(i, &ParamDesc);

			D3D11_INPUT_ELEMENT_DESC elementDesc;
			elementDesc.SemanticName = ParamDesc.SemanticName;
			elementDesc.SemanticIndex = ParamDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			if (ParamDesc.Mask == 1)
			{
				if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (ParamDesc.Mask <= 3)
			{
				if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (ParamDesc.Mask <= 7)
			{
				if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (ParamDesc.Mask <= 15)
			{
				if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			inputLayout[i] = elementDesc;
		}

		VSReflection = nullptr;

		D3D11_INPUT_ELEMENT_DESC* pbuffer = inputLayout.data();
		check(pbuffer);

		r = m_device->CreateInputLayout(
			pbuffer,
			ShaderDesc.InputParameters,
			Blobs.m_vsBlob->GetBufferPointer(),
			Blobs.m_vsBlob->GetBufferSize(),
			&shader.m_inputlayout);

		checkhr(r);
	}
}

void CD3D11Interface::CreateShaderStage(TShader& shader, EShaderStage::Type stage, void* pshadercode, const size_t shaderbinary)
{
	check(shader.m_usedshaderstages[stage] == true);

	HRESULT r;

	switch (stage)
	{
		case EShaderStage::eVS:
		{
			r = m_device->CreateVertexShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages.m_vs);
			checkhr(r);
		}
			break;
		case EShaderStage::eHS:
		{
			r = m_device->CreateHullShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages.m_hs);
			checkhr(r);
		}
			break;
		case EShaderStage::eDS:
		{
			r = m_device->CreateDomainShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages.m_ds);
			checkhr(r);
		}
			break;
		case EShaderStage::eGS:
		{
			r = m_device->CreateGeometryShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages.m_gs);
			checkhr(r);
		}
			break;
		case EShaderStage::ePS:
		{
			r = m_device->CreatePixelShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages.m_ps);
			checkhr(r);
		}
			break;
		case EShaderStage::eCS:
		{
			r = m_device->CreateComputeShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages.m_cs);
			checkhr(r);
		}
		break;
	}
}