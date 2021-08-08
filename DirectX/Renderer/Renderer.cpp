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

void CRenderer::ResetPipeline()
{
	m_D3DInterface->UnbindTargets();
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

	CreateStaticSamplers();

	//sampler override here

	CompileShaders();
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
	m_shaders[0].Initialize<true, false, false, false, true, false>("BasePass");
	//TODO permutate debugLines
	m_shaders[1].Initialize<true, false, false, false, true, false>("DebugLines");

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
}

void CRenderer::Present()
{
	m_D3DInterface->Present();
}