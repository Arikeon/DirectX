#include "Renderer.h"
#include "D3DInterface.h"
#include "D3D11Interface.h"
#include "Stream.h"

void CRenderer::Initialize(TWindow window)
{
	bool bUsesD3D11 = true;
	Stream::ReadIniBool(Stream::GetDefaultDir(), L"API", L"D3D11", bUsesD3D11);

	if (bUsesD3D11)
		m_D3DInterface = new CD3D11Interface;

	m_D3DInterface->InitializeD3D(window);
}

void CRenderer::Update(float delta)
{

}