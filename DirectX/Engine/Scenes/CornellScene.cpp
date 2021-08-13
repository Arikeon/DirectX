#include "CornellScene.h"
#include "FileIO.h"

CCornellScene::CCornellScene()
{
	m_selectedscene = EScene::eCornellScene;
}

CCornellScene::~CCornellScene()
{

}

void CCornellScene::UpdateScene(CRenderer* renderer)
{
	m_debuglines.ClearLines();

	//TODO remove this once the scene is loaded
	//Create Grid
	{
		const float gridY = -3.f;
		const float gridscale = 20.f;

		for (float i = -gridscale; i <= gridscale; ++i)
			m_debuglines.AddDebugLines(float3(i, gridY, gridscale), float3(i, gridY, -gridscale), TColors::Yellow);

		for (float i = -gridscale; i <= gridscale; ++i)
			m_debuglines.AddDebugLines(float3(-gridscale, gridY, i), float3(gridscale, gridY, i), TColors::Yellow);
	}
}

void CCornellScene::LoadScene(CRenderer* renderer)
{
	m_camera.SetStartingPosition(0, 7, -10);

	//Initialize debug lines
	m_debuglines.Initialize(renderer);

	TObject& cornelle = IO::TFileIO::LoadAsset(renderer, m_objects, "CornellAssets", "CornellBox-Glossy.obj");
	//TObject& cornelle = IO::TFileIO::LoadAsset(renderer, m_objects, "windmill", "windmill.obj");
	cornelle.ScaleMeshes(float3(7.f, 7.f, 7.f));
}

void CCornellScene::UnloadScene()
{
	m_objects.clear();
}