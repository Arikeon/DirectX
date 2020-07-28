#include "Renderer.h"
#include "D3D11Interface.h"
#include "Stream.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"

CRenderer::CRenderer() :
	m_D3DInterface(nullptr)
{
}

CRenderer::~CRenderer()
{
	delete m_D3DInterface;
	m_shaders.clear();
	m_vertexbuffers.clear();
	m_indexbuffers.clear();
}

void CRenderer::Initialize(TWindow window)
{
	bool bUsesD3D11 = true;
	Stream::ReadIniBool(Stream::GetDefaultDir(), L"API", L"D3D11", bUsesD3D11);

	if (bUsesD3D11)
		m_D3DInterface = new CD3D11Interface;

	m_D3DInterface->InitializeD3D(window);
	CD3D11Interface::prenderer = this;

	CompileShaders();
}

void CRenderer::CompileShaders()
{
	const int NumGlobalShaders = EShaderList::eCount;
	m_shaders.resize(NumGlobalShaders);
	m_shaders[0].Initialize<true, false, false, false, true, false>("BasePass");

	for (int i = 0; i < NumGlobalShaders; ++i)
	{
		m_D3DInterface->CompileShader(m_shaders[0]);
	}
}

void CRenderer::Render(float delta)
{
	m_D3DInterface->IssueRenderCommands(delta);
	m_D3DInterface->Present();
}

void CRenderer::Restart()
{
	m_vertexbuffers.clear();
	m_indexbuffers.clear();
}