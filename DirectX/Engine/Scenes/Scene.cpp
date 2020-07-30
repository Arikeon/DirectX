#include "Scene.h"
#include "Renderer.h"
#include <array>

void CScene::InitializePasses(CRenderer* renderer)
{

}

void CScene::RenderScene(CRenderer* renderer, float delta)
{
	renderer->ResetPipeline();

	m_basepass.Render(renderer, GetModels(), delta);

}