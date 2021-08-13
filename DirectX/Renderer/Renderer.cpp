#include "Renderer.h"
#include "D3D11Interface.h"
#include "Stream.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#if ENABLE_DEBUG
//#include <ScreenGrab.h>
#include <DirectXTex.h>
#endif //ENABLE_DEBUG

void CRenderer::UnbindRTV()
{
	m_D3DInterface->UnbindRTV();
}

void CRenderer::UnbindSRV(TShader shader)
{
	m_D3DInterface->UnbindSRV(shader);
}

std::vector<ID3D11RenderTargetView*> CRenderer::GetGBuffer()
{
	std::vector<ID3D11RenderTargetView*> arrRTV;

	const int32 numRTVs = EGBufferKeys::eMax;
	check(numRTVs <= RTVSLOTMAX);

	arrRTV.resize(numRTVs);
	for (int32 i = 0; i < numRTVs; ++i)
	{
		arrRTV[i] = GetGBufferRTV(i).m_rtv;
	}
	
	return arrRTV;
}

TextureID CRenderer::CreateTexture(
	unsigned int width,
	unsigned int height,
	unsigned int depth,
	unsigned int arraySize,
	unsigned int mipLevels,
	DXGI_FORMAT format)
{
	m_textures.push_back(m_D3DInterface->CreateTexture(
		width,
		height,
		depth,
		arraySize,
		mipLevels,
		format));

	return TextureID(m_textures.size() - 1);
}

TextureID CRenderer::CreateTexture(ID3D11Resource* resource, ID3D11ShaderResourceView* srv)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	srv->GetDesc(&srvdesc);
	D3D11_SRV_DIMENSION Dimension = srvdesc.ViewDimension;

	HRESULT r = {};

	TD3DTexture texture = {};
	texture.m_srv		= srv;
	texture.m_format	= srvdesc.Format;

	switch (Dimension)
	{
	case D3D_SRV_DIMENSION_TEXTURE1D:
	{
		// no support for this yet
		return -1;
	}
	break;
	case D3D_SRV_DIMENSION_TEXTURE2D:
	{
		r = resource->QueryInterface(IID_ID3D11Texture2D, (void**)&texture.m_tex2D);
		checkhr(r);
		
		D3D11_TEXTURE2D_DESC txDesc;
		texture.m_tex2D->GetDesc(&txDesc);
		
		texture.m_width		= txDesc.Width;
		texture.m_height	= txDesc.Height;
		texture.m_depth		= 1;
		texture.m_arrSize	= txDesc.ArraySize;
		texture.m_mipLevel	= txDesc.MipLevels;
		texture.m_format	= txDesc.Format;
	}
		break;
	case D3D_SRV_DIMENSION_TEXTURE3D:
	{
		r = resource->QueryInterface(IID_ID3D11Texture3D, (void**)&texture.m_tex3D);
		checkhr(r);
		
		D3D11_TEXTURE3D_DESC txDesc;
		texture.m_tex3D->GetDesc(&txDesc);
		
		texture.m_width		= txDesc.Width;
		texture.m_height	= txDesc.Height;
		texture.m_depth		= txDesc.Depth;
		texture.m_arrSize	= 1;
		texture.m_mipLevel	= txDesc.MipLevels;
		texture.m_format	= txDesc.Format;
	}
		break;
	default:
		return -1;
		break;
	}

	DXRelease(resource);

	texture.bIsValid = true;
	m_textures.push_back(texture);
	return (TextureID)(m_textures.size() - 1);
}


void CRenderer::DebugCaptureTexture(TD3DTexture texture, bool isFatal)
{
	//Causes memory leak, only use during Debug
#if ENABLE_DEBUG
	ID3D11ShaderResourceView* srv = texture.m_srv;
	check(srv);

	ID3D11Resource* resource = nullptr;
	srv->GetResource(&resource);
	srv = nullptr;

	HRESULT r = {};
	ScratchImage  image = {};

	r = DirectX::CaptureTexture(m_D3DInterface->m_device, m_D3DInterface->m_context, resource, image);
	DXRelease(resource);

	if (r == S_OK)
	{
		std::wstring dir = Algorithm::ChopLast(Algorithm::GetExecutablePath(), L"\\", 4) + L"\\DebugTex.dds";

		r = DirectX::SaveToDDSFile(
			image.GetImages(),
			1,
			image.GetMetadata(),
			DDS_FLAGS_NONE,
			dir.c_str());

		if (r == S_OK)
			CONSOLE_LOG(L"saved dbug texture...");
	}

	if (isFatal) // will leak
		exit(0);
#endif
}

CRenderer::CRenderer() :
	m_D3DInterface(nullptr)
{
}

CRenderer::~CRenderer()
{
	delete m_D3DInterface;

	D3DRelease(m_GBuffer);
	D3DArrayRelease(m_shaders);
	D3DArrayRelease(m_vertexbuffers);
	D3DArrayRelease(m_indexbuffers);
	D3DArrayRelease(m_rtvs);
	D3DArrayRelease(m_textures);
	D3DArrayRelease(m_samplers);
	D3DArrayRelease(m_depthtargets);
	DXArrayRelease(m_rasterizerstates);
}

void CRenderer::Initialize(TWindow window)
{
	bool bUsesD3D11 = true;
	Stream::ReadIniBool(Stream::GetDefaultDir(), L"API", L"D3D11", bUsesD3D11);

	if (bUsesD3D11)
		m_D3DInterface = new CD3D11Interface;
	CD3D11Interface::prenderer = this;

	m_D3DInterface->InitializeD3D(window);

	//Start creating global resources ex: gbuffers, samplers
	CreateGBufferRenderTargets(window);
	CreateStaticTextures();
	CreateStaticSamplers();

	//sampler override here

	CompileShaders();
}

void CRenderer::CreateGBufferRenderTargets(TWindow window, bool bResize)
{
	if (bResize)
	{
		// release and re-create gbuffer with new width + height
	}

	ID3D11Device* device = m_D3DInterface->GetDevice();
	HRESULT r = {};

	//________________BaseColor________________
	{
		TD3DTexture tex = {};
		tex.m_name = "GBuffer_BaseColorRenderTarget";

		D3D11_TEXTURE2D_DESC desc	= {};
		desc.Width					= window.m_width;
		desc.Height					= window.m_height;
		desc.ArraySize				= 1;
		desc.MipLevels				= 1;
		desc.Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage					= D3D11_USAGE_DEFAULT;
		desc.BindFlags				= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count		= 1;

		tex.SetDesc(desc);

		r = device->CreateTexture2D(&desc, nullptr, &tex.m_tex2D);
		checkhr(r);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format							= desc.Format;
		srvDesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip		= 0;
		srvDesc.Texture2D.MipLevels				= -1;

		r = device->CreateShaderResourceView(tex.m_tex2D, &srvDesc, &tex.m_srv);
		checkhr(r);
		
		TD3DRenderTarget rtv = {};
		device->CreateRenderTargetView(tex.m_tex2D, nullptr, &rtv.m_rtv);
		checkhr(r);

		rtv.bIsValid = true;
		tex.bIsValid = true;

		rtv.m_textureid = (TextureID)Algorithm::ArrPush_Back(m_textures, tex);
		m_GBuffer.AddRTV(rtv, (int32)EGBufferKeys::eBaseColor);
	}
	//________________WorldNormal________________
	{
		TD3DTexture tex = {};
		tex.m_name = "GBuffer_WorldNormalRenderTarget";

		D3D11_TEXTURE2D_DESC desc	= {};
		desc.Width					= window.m_width;
		desc.Height					= window.m_height;
		desc.ArraySize				= 1;
		desc.MipLevels				= 1;
		desc.Format					= DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.Usage					= D3D11_USAGE_DEFAULT;
		desc.BindFlags				= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count		= 1;

		tex.SetDesc(desc);

		r = device->CreateTexture2D(&desc, nullptr, &tex.m_tex2D);
		checkhr(r);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format							= desc.Format;
		srvDesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip		= 0;
		srvDesc.Texture2D.MipLevels				= -1;
		
		r = device->CreateShaderResourceView(tex.m_tex2D, &srvDesc, &tex.m_srv);
		checkhr(r);

		TD3DRenderTarget rtv = {};
		device->CreateRenderTargetView(tex.m_tex2D, nullptr, &rtv.m_rtv);
		checkhr(r);

		rtv.bIsValid = true;
		tex.bIsValid = true;

		rtv.m_textureid = (TextureID)Algorithm::ArrPush_Back(m_textures, tex);
		m_GBuffer.AddRTV(rtv, (int32)EGBufferKeys::eWorldNormal);
	}
	//________________Roughness________________
	{
		TD3DTexture tex = {};
		tex.m_name = "GBuffer_RoughnessRenderTarget";

		D3D11_TEXTURE2D_DESC desc	= {};
		desc.Width					= window.m_width;
		desc.Height					= window.m_height;
		desc.ArraySize				= 1;
		desc.MipLevels				= 1;
		desc.Format					= DXGI_FORMAT_R32_FLOAT;
		desc.Usage					= D3D11_USAGE_DEFAULT;
		desc.BindFlags				= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count		= 1;

		tex.SetDesc(desc);

		r = device->CreateTexture2D(&desc, nullptr, &tex.m_tex2D);
		checkhr(r);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format							= desc.Format;
		srvDesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip		= 0;
		srvDesc.Texture2D.MipLevels				= -1;
		
		r = device->CreateShaderResourceView(tex.m_tex2D, &srvDesc, &tex.m_srv);
		checkhr(r);

		TD3DRenderTarget rtv = {};
		device->CreateRenderTargetView(tex.m_tex2D, nullptr, &rtv.m_rtv);
		checkhr(r);

		rtv.bIsValid = true;
		tex.bIsValid = true;

		rtv.m_textureid = (TextureID)Algorithm::ArrPush_Back(m_textures, tex);
		m_GBuffer.AddRTV(rtv, (int32)EGBufferKeys::eRoughness);
	}
	//________________Metallic________________
	{
		TD3DTexture tex = {};
		tex.m_name = "GBuffer_MetallicRenderTarget";

		D3D11_TEXTURE2D_DESC desc	= {};
		desc.Width					= window.m_width;
		desc.Height					= window.m_height;
		desc.ArraySize				= 1;
		desc.MipLevels				= 1;
		desc.Format					= DXGI_FORMAT_R32_FLOAT;
		desc.Usage					= D3D11_USAGE_DEFAULT;
		desc.BindFlags				= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count		= 1;

		tex.SetDesc(desc);

		r = device->CreateTexture2D(&desc, nullptr, &tex.m_tex2D);
		checkhr(r);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format							= desc.Format;
		srvDesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip		= 0;
		srvDesc.Texture2D.MipLevels				= -1;
		
		r = device->CreateShaderResourceView(tex.m_tex2D, &srvDesc, &tex.m_srv);
		checkhr(r);

		TD3DRenderTarget rtv = {};
		device->CreateRenderTargetView(tex.m_tex2D, nullptr, &rtv.m_rtv);
		checkhr(r);

		rtv.bIsValid = true;
		tex.bIsValid = true;

		rtv.m_textureid = (TextureID)Algorithm::ArrPush_Back(m_textures, tex);
		m_GBuffer.AddRTV(rtv, (int32)EGBufferKeys::eMetallic);
	}

	check(m_GBuffer.CountValid() == EGBufferKeys::eMax);
	m_GBuffer.bIsValid = true;
}

void CRenderer::CreateStaticTextures()
{
	//TODO add default textures
}

void CRenderer::CreateStaticSamplers()
{
	//make sure these are the first samplers
	check(m_samplers.size() == 0);

	//Default
	m_samplers.push_back(GetD3DInterface()->CreateSampler(
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_NEVER));
	
	check(m_samplers.size() == EStaticSamplerKey::eDefault + 1);
}

void CRenderer::CompileShaders()
{
	const int NumGlobalShaders = EShaderList::eCount;
	m_shaders.resize(NumGlobalShaders);


	std::vector<TShaderPermutationKey> BasePassPermutations = { 
		{"USE_TEXTURE_DIFFUSE", 0, 1 },
		{"USE_TEXTURE_NORMAL", 0, 1} };

	std::vector<TShaderPermutationKey> DeferredLightingPermutations = {
		{"LIGHT_TYPE_DIRECTIONAL", 0, 1 },
		{"LIGHT_TYPE_POINT", 0, 1 },
		{"LIGHT_TYPE_SPOT", 0, 1 }};
	
	m_shaders[EShaderList::eBasePass].			Initialize<true, false, false, false, true, false>("BasePass", BasePassPermutations);
	m_shaders[EShaderList::eDebugBasePass].		Initialize<true, false, false, false, true, false>("DebugLines");
	m_shaders[EShaderList::eScreenQuad].		Initialize<true, false, false, false, false, false>("ScreenQuad");
	m_shaders[EShaderList::eDeferredLighting].	Initialize<false, false, false, false, true, false>("DeferredLighting", DeferredLightingPermutations);

	for (int i = 0; i < NumGlobalShaders; ++i)
	{
		m_D3DInterface->CompileShader(m_shaders[i]);
	}
}
void CRenderer::Restart()
{
	m_vertexbuffers.clear();
	m_indexbuffers.clear();
}

void CRenderer::Clear()
{
	m_D3DInterface->ClearBackBuffer();
	m_D3DInterface->ClearGBuffer(m_GBuffer);
}

void CRenderer::Present()
{
	m_D3DInterface->Present();
}