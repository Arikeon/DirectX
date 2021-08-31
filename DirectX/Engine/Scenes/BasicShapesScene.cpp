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
	skybox.m_models[0].m_transform.m_rotation.z = 90.f;
	m_objects.push_back(skybox);

	//______________teapots
	const int32 numteapots = 10;
	TObject teapots;
	IO::TFileIO::LoadAsset(renderer, "Teapot", "utah-teapot.obj", teapots);

	int32 i = 0, j = 0;
	float xPos = -10.f, zPos = -10.f;

	for (; i < numteapots / 2; ++i, xPos += 5.f)
	{
		for (; j < 2; ++j, zPos += 2.f)
		{
			TTransform transform = teapots.m_models[0].m_transform;
			transform.m_position.x = xPos;
			transform.m_position.z = zPos;

			teapots.m_models[0].m_meshes[0].m_instanceTransforms.push_back(transform);
		}

		j = 0;
	}

	m_objects.push_back(teapots);
}

void CBasicShapesScene::UnloadScene()
{
	m_objects.clear();
}