#include "D3D11Interface.h"
#include "HLSLGlue.h"
#include "Mesh.h"
#include "Renderer.h"
#include <d3dcompiler.h>

//Resource Getter
CRenderer* CD3D11Interface::prenderer = nullptr;

CD3D11Interface::CD3D11Interface() :
	m_swapchain(nullptr),
	m_device(nullptr),
	m_context(nullptr),
#if ENABLE_DXGI_DEBUG
	m_dxgidebug(nullptr)
#endif
{
}

CD3D11Interface::~CD3D11Interface()
{
	prenderer = nullptr;
	DXRelease(m_swapchain);
	DXRelease(m_device);
	DXRelease(m_context);
#if ENABLE_DXGI_DEBUG
	DXRelease(m_dxgidebug);
#endif
}

void CD3D11Interface::ClearBackBuffer()
{
	const float defaultcolor[4] = { 0.2f, 0.2f, 0.2f, 0 };
	m_context->ClearRenderTargetView(prenderer->GetRenderTarget(0).m_rtv, defaultcolor);
	m_context->ClearDepthStencilView(prenderer->GetDepthTarget(0).m_dsv, D3D11_CLEAR_DEPTH, 1, 1);
}

void CD3D11Interface::ClearGBuffer(TGBuffer& GBuffer)
{
	const FLOAT clearColor[4] = { 0.f, 0.f, 0.f, 0.f };

	const int32 numRTVs = EGBufferKeys::eMax;
	ID3D11RenderTargetView* rtvs[numRTVs];

	for (int32 i = 0; i < numRTVs; ++i)
	{
		rtvs[i] = GBuffer.GetRTV(i).m_rtv;
	}

	for (int32 i = 0; i < numRTVs; ++i)
	{
		m_context->ClearRenderTargetView(rtvs[i], clearColor);
	}
}

void CD3D11Interface::Present()
{
	m_swapchain->Present(1, 0);
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

		UINT DeviceFlags =
#if ENABLE_DXGI_DEBUG
			//TODO switch to multithreaded
			D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG;
#else
			D3D11_CREATE_DEVICE_SINGLETHREADED;
#endif

		r = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			DeviceFlags,
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
		TD3DRenderTarget MainRTV;
		TD3DTexture rtvTexture = {};

		m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&rtvTexture.m_tex2D);
		r = m_device->CreateRenderTargetView(rtvTexture.m_tex2D, nullptr, &MainRTV.m_rtv);
		checkhr(r);

		check(MainRTV.m_rtv && rtvTexture.m_tex2D);

		rtvTexture.bIsValid = true;
		MainRTV.m_textureid = (TextureID)Algorithm::ArrPush_Back(prenderer->m_textures, rtvTexture);

		MainRTV.bIsValid = true;
		check(Algorithm::ArrPush_Back(prenderer->m_rtvs, MainRTV) == ERenderTargetKey::eBackBufeer);
	}

	//Initialize zbuffer ref
	{
		TD3DDepthTarget MainDepth;
		TD3DTexture dsvTexture = {};

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

		r = m_device->CreateTexture2D(&DepthTextureDesc, 0, &dsvTexture.m_tex2D);
		checkhr(r);

		D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
		DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

		r = m_device->CreateDepthStencilView(dsvTexture.m_tex2D, &DepthStencilViewDesc, &MainDepth.m_dsv);
		checkhr(r);

		D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
		DepthStencilDesc.DepthEnable = true;
		DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		check(MainDepth.m_dsv && dsvTexture.m_tex2D);

		dsvTexture.bIsValid = true;
		MainDepth.m_textureid = (TextureID)Algorithm::ArrPush_Back(prenderer->m_textures, dsvTexture);

		MainDepth.bIsValid = true;
		check(Algorithm::ArrPush_Back(prenderer->m_depthtargets, MainDepth) == 0);
	}

	//Initialize rasterizerstate
	//TODO pre-alloc all states
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

		r = m_device->CreateRasterizerState(&RasterizerDesc, &prenderer->m_rasterizerstates[0]);
		checkhr(r);
	}

	//Initial VP
	//TODO pre-alloc all views
	{
		prenderer->m_views[0].TopLeftX = (float)window.m_posX;
		prenderer->m_views[0].TopLeftY = (float)window.m_posY;
		prenderer->m_views[0].Height = (float)window.m_height;
		prenderer->m_views[0].Width = (float)window.m_width;
		prenderer->m_views[0].MinDepth = (float)m_nearplane;
		prenderer->m_views[0].MaxDepth = (float)m_farplane;
	}

	for (int i = 0; i < (int)prenderer->m_rasterizerstates.size(); i++)
	{
		if (prenderer->m_rasterizerstates[i] == nullptr)
			bSuccess = false;
	}

	if (bSuccess == false)
	{
		CONSOLE_LOG(L"Failed to initialized d3d");
		exit(1);
	}
}

void CD3D11Interface::CompileShader(TShader& shader)
{
#if ENABLE_DEBUG
	const UINT SHADER_COMPILE_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS
		| D3DCOMPILE_DEBUG
		| D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	const UINT SHADER_COMPILE_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

	int NumberOfStages = 0;

	HRESULT r;
	TD3DBlobs Blobs;

	PermutationArray shaderPermutations = shader.m_permutations.m_pArray;

	check(shader.m_permutations.bIsValid());

	shader.m_shaderstages.resize(shaderPermutations.size());
	shader.m_shaderreflections.resize(shaderPermutations.size());

	for (int pIndex = 0; pIndex < shaderPermutations.size(); ++pIndex)
	{
		std::vector<PermutationValue> permutationsValue = shaderPermutations[pIndex];

		for (int i = 0; i < EShaderStage::eCount; ++i)
		{
			EShaderStage::Type stage = (EShaderStage::Type)i;

			if (shader.m_usedshaderstages[(EShaderStage::Type)stage] == false)
				continue;
			NumberOfStages++;

			std::string pathstr = shader.GetDirectoryFromStage((EShaderStage::Type)stage);
			std::wstring pathwstr = Algorithm::string_to_wstring(pathstr);

			ID3DBlob* errmsg, * pblob;
			std::vector<D3D_SHADER_MACRO> NullTerminatedMacro = {};

			auto& permutation = shaderPermutations[pIndex];
			for (auto valueArr : permutation)
			{
				D3D_SHADER_MACRO newMacro = {};

				std::string defStr = std::to_string(valueArr.second);

				size_t nameSize = valueArr.first.size();
				size_t definitionSize = defStr.size();

				char* name = new char[nameSize + 1];
				char* definition = new char[definitionSize + 1];

				strcpy_s(name, nameSize + 1, valueArr.first.c_str());
				strcpy_s(definition, definitionSize + 1, defStr.c_str());

				name[nameSize] = '\0';
				definition[definitionSize] = '\0';

				newMacro.Name = name;
				newMacro.Definition = definition;
				NullTerminatedMacro.push_back(newMacro);
			}

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

			for (auto& macro : NullTerminatedMacro)
			{
				delete macro.Definition;
				delete macro.Name;
			}

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

			CreateShaderStage(
				shader,
				stage, 
				Blobs.m_stageblob[stage]->GetBufferPointer(), 
				Blobs.m_stageblob[stage]->GetBufferSize(),
				pIndex);


			//set reflections
			{
				for (unsigned stage = EShaderStage::eVS; stage < EShaderStage::eCount; ++stage)
				{
					if (Blobs.m_stageblob[stage] != nullptr)
					{
						void** pbuffer = reinterpret_cast<void**>(&shader.m_shaderreflections[pIndex].m_stagerreflection[stage]);
						r = D3DReflect(Blobs.m_stageblob[stage]->GetBufferPointer(), Blobs.m_stageblob[stage]->GetBufferSize(), IID_ID3D11ShaderReflection, pbuffer);
						checkhr(r);
					}
				}
			}
		}
	}

	//Atleast one shader have to compile
	check(NumberOfStages > 0);

	//Allocate textures
	for (int stage = 0; stage < (int)EShaderStage::eCount; ++stage)
	{
		int texslot = 0, sampslot = 0, uavslot = 0;

		ID3D11ShaderReflection* reflection = shader.m_shaderreflections[0].m_stagerreflection[stage];

		if (reflection)
		{
			D3D11_SHADER_DESC shaderdesc = {};
			reflection->GetDesc(&shaderdesc);

			for (int boundrescount = 0; boundrescount < (int)shaderdesc.BoundResources; ++boundrescount)
			{
				D3D11_SHADER_INPUT_BIND_DESC shaderinputdesc = {};
				reflection->GetResourceBindingDesc(boundrescount, &shaderinputdesc);

				switch (shaderinputdesc.Type)
				{
				case D3D_SIT_CBUFFER:
						break;
				case D3D_SIT_SAMPLER:
				{
					TSamplerBinding sampbinding;
					sampbinding.m_shaderStage = (EShaderStage::Type)stage;
					sampbinding.m_samplerSlot = sampslot++;
					shader.m_samplerbindings.push_back(sampbinding);
					shader.m_shadersamplerlookup[shaderinputdesc.Name] = static_cast<int>(shader.m_samplerbindings.size() - 1);
				}
				break;
				case D3D_SIT_TEXTURE:
				{
					TTextureBinding textbinding;
					textbinding.m_shaderStage = (EShaderStage::Type)stage;
					textbinding.m_textureSlot = sampslot++;
					shader.m_texturebindings.push_back(textbinding);
					shader.m_shadertexturelookup[shaderinputdesc.Name] = static_cast<int>(shader.m_texturebindings.size() - 1);
				}
				break;
				case D3D_SIT_UAV_RWTYPED:
				{
					TTextureBinding textbinding;
					textbinding.m_shaderStage = (EShaderStage::Type)stage;
					textbinding.m_textureSlot = uavslot++;
					shader.m_texturebindings.push_back(textbinding);
					shader.m_shadertexturelookup[shaderinputdesc.Name] = static_cast<int>(shader.m_texturebindings.size() - 1);
				}
				break;

				}
			}
		}
	}

	//Allocate cbuffers (CPU&GPU)
	{
		for (unsigned stage = EShaderStage::eVS; stage < EShaderStage::eCount; ++stage)
		{
			//assumes no permutation on cbuffers
			ID3D11ShaderReflection* Reflection = shader.m_shaderreflections[0].m_stagerreflection[stage];
			if (Reflection)
			{
				D3D11_SHADER_DESC shaderdesc;
				Reflection->GetDesc(&shaderdesc);

				int bufferslot = 0;
				for (int cbuffercount = 0; cbuffercount < (int)shaderdesc.ConstantBuffers; ++cbuffercount)
				{
					TD3DConstantBufferLayout Layout;
					Layout.m_bufferSize = 0;
					ID3D11ShaderReflectionConstantBuffer* pcbuffer = Reflection->GetConstantBufferByIndex(cbuffercount);
					pcbuffer->GetDesc(&Layout.m_desc);

					for (int cbuffervariable = 0; cbuffervariable < (int)Layout.m_desc.Variables; ++cbuffervariable)
					{
						ID3D11ShaderReflectionVariable* pVariable = pcbuffer->GetVariableByIndex(cbuffervariable);
						D3D11_SHADER_VARIABLE_DESC variabledesc;
						pVariable->GetDesc(&variabledesc);
						Layout.m_variables.push_back(variabledesc);

						ID3D11ShaderReflectionType* ptype = pVariable->GetType();
						D3D11_SHADER_TYPE_DESC typedesc;
						ptype->GetDesc(&typedesc);
						Layout.m_types.push_back(typedesc);

						Layout.m_bufferSize += variabledesc.Size;
					}
					Layout.m_stage = (EShaderStage::Type)stage;
					Layout.m_bufferSlot = bufferslot++;

					shader.m_constantbufferlayouts.push_back(Layout);
				}
			}
		}

		//CPU
		int cbufferregister = 0;
		for (int cblayout = 0; cblayout < (int)shader.m_constantbufferlayouts.size(); ++cblayout)
		{
			TD3DConstantBufferLayout& layout = shader.m_constantbufferlayouts[cblayout];

			std::vector<CPUConstantID> cpubufferID;

			for (int varcount = 0; varcount < (int)layout.m_variables.size(); ++varcount)
			{
				D3D11_SHADER_VARIABLE_DESC vardesc = layout.m_variables[varcount];

				TCPUConstant c;
				CPUConstantID c_id = static_cast<CPUConstantID>(shader.m_CPUconstantbuffers.size());

				c.m_name = vardesc.Name;
				c.m_size = vardesc.Size;
				c.m_pdata = new char[c.m_size];
				memset(c.m_pdata, 0, c.m_size);
				shader.m_constantbuffermap.push_back(std::make_pair(cbufferregister, c_id));
				shader.m_CPUconstantbuffers.push_back(c);
			}
			++cbufferregister;
		}

		//GPU
		{
			D3D11_BUFFER_DESC cbufferDesc = {};
			cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbufferDesc.MiscFlags = 0;
			cbufferDesc.StructureByteStride = 0;

			for (int cblayout = 0; cblayout < shader.m_constantbufferlayouts.size(); ++cblayout)
			{
				TD3DConstantBufferLayout& layout = shader.m_constantbufferlayouts[cblayout];

				TConstantBufferBinding cbufferbinding;
				cbufferDesc.ByteWidth = layout.m_desc.Size;

				r = m_device->CreateBuffer(&cbufferDesc, NULL, &cbufferbinding.m_pdata);
				checkhr(r);

				cbufferbinding.m_dirty = true;
				cbufferbinding.m_shaderstage = layout.m_stage;
				cbufferbinding.m_bufferslot = layout.m_bufferSlot;
				shader.m_constantbuffers.push_back(cbufferbinding);
			}
		}
	}

	//Compile InputLayout if it uses vertex shader
	if (shader.m_usedshaderstages[EShaderStage::eVS] == true)
	{
		ID3D11ShaderReflection* VSReflection = shader.m_shaderreflections[0].m_vsreflection;

		D3D11_SHADER_DESC shaderdesc = {};
		VSReflection->GetDesc(&shaderdesc);
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout(shaderdesc.InputParameters);
		D3D_PRIMITIVE primitiveDesc = shaderdesc.InputPrimitive;

		shader.m_info.m_instructioncount = shaderdesc.InstructionCount;

		for (int i = 0; i < (int)shaderdesc.InputParameters; ++i)
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
			shaderdesc.InputParameters,
			Blobs.m_vsBlob->GetBufferPointer(),
			Blobs.m_vsBlob->GetBufferSize(),
			&shader.m_inputlayout);

		checkhr(r);
	}

	for (int i = 0; i < EShaderStage::eCount; ++i)
	{
		DXRelease(Blobs.m_stageblob[i]);
	}
}

TD3DSampler CD3D11Interface::CreateSampler(
	D3D11_TEXTURE_ADDRESS_MODE aU,
	D3D11_TEXTURE_ADDRESS_MODE aV,
	D3D11_TEXTURE_ADDRESS_MODE aW,
	D3D11_COMPARISON_FUNC comparisonFunc,
	std::string debugname
)
{
	D3D11_SAMPLER_DESC desc = {};
	desc.Filter				= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU			= aU;
	desc.AddressV			= aV;
	desc.AddressW			= aW;
	desc.ComparisonFunc		= comparisonFunc;
	desc.MinLOD				= 0;
	desc.MaxLOD				= D3D11_FLOAT32_MAX;

	ID3D11SamplerState* d3dsampler = nullptr;

	HRESULT r = {};
	this->m_device->CreateSamplerState(&desc, &d3dsampler);
	checkhr(r);

	TD3DSampler sampler = {};
	sampler.m_name = debugname;
	sampler.m_samplerstate = d3dsampler;

	sampler.bIsValid = true;
	return sampler;
}

TD3DTexture CD3D11Interface::CreateTexture(
	unsigned int width,
	unsigned int height,
	unsigned int depth,
	unsigned int arraySize,
	unsigned int mipLevels,
	DXGI_FORMAT format)
{
	//TODO add support for RW and UAV
	HRESULT r = {};

	TD3DTexture texture = {};

	D3D11_TEXTURE2D_DESC TextureDesc = {};
	TextureDesc.Height = width;
	TextureDesc.Width = height;
	TextureDesc.MipLevels = mipLevels;
	TextureDesc.ArraySize = arraySize;
	TextureDesc.Format = format;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = false;

	texture.SetDesc(TextureDesc);
	
	r = m_device->CreateTexture2D(&TextureDesc, 0, &texture.m_tex2D);
	checkhr(r);


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;


	const bool bIsCubed = depth > 1;
	const bool bIsArray = arraySize > 1; //TODO add support for this

	if (bIsCubed)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = mipLevels;
		r = m_device->CreateShaderResourceView(texture.m_tex2D, &srvDesc, &texture.m_srv);
	}
	else
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = mipLevels;
		r = m_device->CreateShaderResourceView(texture.m_tex2D, &srvDesc, &texture.m_srv);
	}
	checkhr(r);

	texture.bIsValid = true;
	return texture;
}

void CD3D11Interface::CreateShaderStage(
	TShader& shader,
	EShaderStage::Type stage,
	void* pshadercode,
	const size_t shaderbinary,
	int32 permutationIndex = 0)
{
	check(shader.m_usedshaderstages[stage] == true);

	HRESULT r;

	switch (stage)
	{
		case EShaderStage::eVS:
		{
			r = m_device->CreateVertexShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages[permutationIndex].m_vs);
			checkhr(r);
		}
			break;
		case EShaderStage::eHS:
		{
			r = m_device->CreateHullShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages[permutationIndex].m_hs);
			checkhr(r);
		}
			break;
		case EShaderStage::eDS:
		{
			r = m_device->CreateDomainShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages[permutationIndex].m_ds);
			checkhr(r);
		}
			break;
		case EShaderStage::eGS:
		{
			r = m_device->CreateGeometryShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages[permutationIndex].m_gs);
			checkhr(r);
		}
			break;
		case EShaderStage::ePS:
		{
			r = m_device->CreatePixelShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages[permutationIndex].m_ps);
			checkhr(r);
		}
			break;
		case EShaderStage::eCS:
		{
			r = m_device->CreateComputeShader(pshadercode, shaderbinary, NULL, &shader.m_shaderstages[permutationIndex].m_cs);
			checkhr(r);
		}
		break;
	}
}

void CD3D11Interface::UnbindRTV()
{
	ID3D11RenderTargetView* NullRTVs[RTV_MAX] = {};
	ID3D11DepthStencilView* NullDSV = {};
	m_context->OMSetRenderTargets(RTV_MAX, NullRTVs, NullDSV);

	const FLOAT blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_context->OMSetBlendState(nullptr, blendFactor, UINT_MAX);
}

void CD3D11Interface::UnbindSRV(TShader shader)
{
	if (shader.m_texturebindings.size() == 0)
		return;

	ID3D11ShaderResourceView* NullSRVs[SRV_MAX] = {};

	for (int32 i = 0; i < shader.m_texturebindings.size(); ++i)
	{
		TTextureBinding texbinding = shader.m_texturebindings[i];

		switch (texbinding.m_shaderStage)
		{
		case EShaderStage::eVS:
			m_context->VSSetShaderResources(0, SRV_MAX, NullSRVs);
			break;
		case EShaderStage::eGS:
			m_context->GSSetShaderResources(0, SRV_MAX, NullSRVs);
			break;
		case EShaderStage::eHS:
			m_context->HSSetShaderResources(0, SRV_MAX, NullSRVs);
			break;
		case EShaderStage::eDS:
			m_context->DSSetShaderResources(0, SRV_MAX, NullSRVs);
			break;
		case EShaderStage::ePS:
			m_context->PSSetShaderResources(0, SRV_MAX, NullSRVs);
			break;
		case EShaderStage::eCS:
		{
			ID3D11UnorderedAccessView* NullUAVs[SRV_MAX] = {};
			m_context->CSSetShaderResources(texbinding.m_textureSlot, SRV_MAX, NullSRVs);
			m_context->CSSetUnorderedAccessViews(texbinding.m_textureSlot, SRV_MAX, NullUAVs, nullptr);
		}
			break;
		}

	}
}

void CD3D11Interface::ResizeViewPorts(TWindow window)
{

}
