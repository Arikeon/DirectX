#include "Scene.h"
#include "Renderer.h"
#include <array>

void CScene::InitializePasses(CRenderer* renderer)
{
}

void CScene::RenderScene(CRenderer* renderer, TWindow window, float delta)
{
	renderer->ResetPipeline();

	m_camera.Update(delta);

	m_basepass.Render(renderer, GetModels(), m_debuglines, m_camera, window,delta);
}