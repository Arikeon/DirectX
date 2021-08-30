#include "BasicShapesScene.h"
#include "Renderer.h"
#include "StructCollection.h"
#include "Model.h"
#include "Mesh.h"
#include "FileIO.h"

CBasicShapesScene::CBasicShapesScene()
{
	m_selectedscene = EScene::eBasicShapesScene;
}


CBasicShapesScene::~CBasicShapesScene()
{

}

void CBasicShapesScene::UpdateScene(CRenderer* renderer)
{
	m_debuglines.ClearLines();

	//Create Grid
	{
		const float gridY = -3.f;
		const float gridscale = 20.f;

		for (float i = -gridscale; i <= gridscale; ++i)
			m_debuglines.AddDebugLines(float3(i, gridY, gridscale), float3(i, gridY, -gridscale), TColors::White);

		for (float i = -gridscale; i <= gridscale; ++i)
			m_debuglines.AddDebugLines(float3(-gridscale, gridY, i), float3(gridscale, gridY, i), TColors::White);
	}
}

void CBasicShapesScene::LoadScene(CRenderer* renderer)
{
	check(renderer);

	m_camera.SetStartingPosition(0, 7, -10);

	//Initialize debug lines
	m_debuglines.Initialize(renderer);

	//______________skybox
	TObject skybox;
	float SkyboxScale = 9999.f;
	int32 SkyboxTexture = IO::TFileIO::LoadTextureFromFile(renderer, "SkyboxAssets", "skybox.png");
	IO::TFileIO::CreateCube(renderer, skybox, SkyboxScale, SkyboxTexture);
	skybox.m_models[0].m_rasterizationState = ERasterizerStates::eFillSolidCullBack;
	m_objects.push_back(skybox);

	//______________spheres
	//const int32 numSpheres = 1;
	TObject spheres;
	IO::TFileIO::LoadAsset(renderer, "Shapes", "Sphere.obj", spheres);
	m_objects.push_back(spheres);

}

void CBasicShapesScene::UnloadScene()
{
	m_objects.clear();
}