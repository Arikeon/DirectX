#include "CornellScene.h"
#include "FileIO.h"

CCornellScene::CCornellScene()
{
	m_selectedscene = EScene::eCornellScene;
}

CCornellScene::~CCornellScene()
{

}

void CCornellScene::UpdateScene(CRenderer* renderer, float delta)
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

	TObject skybox, cornelle;




	//______________cornelle
	//IO::TFileIO::LoadAsset(renderer, "CornellAssets", "CornellBox-Sphere.obj", cornelle);
	//cornelle.ScaleMeshes(float3(7.f, 7.f, 7.f));
	//
	////Cornelle has inward facing walls
	//for (int32 i = 0; i < (int32)cornelle.m_models.size(); ++i)
	//{
	//	TModel& model = cornelle.m_models[i];
	//	model.m_rasterizationState = ERasterizerStates::eFillSolidCullFront;
	//}
	//m_objects.push_back(cornelle);


	//TDirectionalLight DirectionalLight = {};
	//DirectionalLight.m_color = float3(1.f, 1.f, 1.f);
	//DirectionalLight.m_intensity = 0.2f;
	//DirectionalLight.
}

void CCornellScene::UnloadScene()
{
	m_objects.clear();
}