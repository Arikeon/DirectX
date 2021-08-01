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

void CRenderer::DebugCaptureTexture(TD3DTexture texture, bool isFatal)
{
#if ENABLE_DEBUG
	ID3D11ShaderResourceView* srv = texture.m_srv;
	check(srv);

	ID3D11Resource* resource = nullptr;
	srv->GetResource(&resource);

	HRESULT r = {};
	ScratchImage  image = {};

	r = DirectX::CaptureTexture(m_D3DInterface->m_device, m_D3DInterface->m_context, resource, image);

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

	CompileShaders();
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