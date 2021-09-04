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

void CBasicShapesScene::UpdateScene(CRenderer* renderer, float delta)
{
	m_debuglines.ClearLines();
	m_debuglines.AddDebugLines(float3(0, 0, 0), float3(10, 10, 10), TColors::Blue);
	//Random
	{
		TObject& skybox = m_objects[0];
		skybox.m_models[0].m_transform.m_rotation.y += .0006f * delta;
		skybox.m_models[0].m_transform.m_rotation.x += .0006f * delta;
	}

#if 1
	float3 CameraPos;
	XMStoreFloat3(&CameraPos, m_camera.m_pos);
	float3 SpherePos = m_objects[1].m_models[0].m_transform.m_position;

	float inner = (float)pow((CameraPos.x - SpherePos.x), 2) + pow((CameraPos.y - SpherePos.y), 2) + pow((CameraPos.z - SpherePos.z), 2);

	float distancefromSphere = sqrt(inner);

	CONSOLE_LOG(std::to_wstring(distancefromSphere));
#endif
}

void CBasicShapesScene::LoadScene(CRenderer* renderer)
{
	check(renderer);

	m_camera.SetStartingPosition(0, 0, -20);

	//Initialize debug lines
	m_debuglines.Initialize(renderer);

	//______________skybox
	//https://wwwtyro.github.io/space-3d/
	TObject skybox;
	float SkyboxScale = 9999.f;
	int32 SkyboxTexture = IO::TFileIO::LoadTextureFromFile(renderer, "SkyboxAssets", "skybox.png");
	IO::TFileIO::CreateCube(renderer, skybox, SkyboxScale, SkyboxTexture);
	skybox.m_models[0].m_rasterizationState = ERasterizerStates::eFillSolidCullBack;
	m_objects.push_back(skybox);

	//______________teapots
	const int32 numteapots = 10;
	TObject teapots;
	//slow
	//IO::TFileIO::LoadAsset(renderer, "Teapot", "utah-teapot.obj", teapots);
	IO::TFileIO::LoadAsset(renderer, "Shapes", "cube.obj", teapots);
	float boxScale = 5;
	teapots.ScaleMeshes({ boxScale, boxScale, boxScale });

	//int32 i = 0, j = 0;
	//float xPos = -10.f, zPos = 0.f;
	//
	//for (; i < 2; ++i, zPos += 6.f)
	//{
	//	for (; j < numteapots / 2; ++j, xPos += 6.f)
	//	{
	//		TTransform transform = teapots.m_models[0].m_transform;
	//		transform.m_position.x = xPos;
	//		transform.m_position.z = zPos;
	//
	//		teapots.m_models[0].m_meshes[0].m_instanceTransforms.push_back(transform);
	//	}
	//
	//	xPos = -10.f;
	//
	//	j = 0;
	//}

	m_objects.push_back(teapots);

	//______________lights
	TDirectionalLight Star;
	Star.m_color = TColors::White;
	Star.m_intensity = 0.5f;
	Star.m_direction.z = -1.f;
	Star.m_direction.x = -30.f;
	Star.m_lightType = ELightType::eDirectional;

	m_Lights.push_back(Star);
}

void CBasicShapesScene::UnloadScene()
{
	m_objects.clear();
}