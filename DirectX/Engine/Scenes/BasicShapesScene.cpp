#include "BasicShapesScene.h"
#include "Renderer.h"
#include "StructCollection.h"
#include "Model.h"
#include "Mesh.h"
#include "FileIO.h"
#include "Input.h"

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
	if(0)
	{
		TObject& skybox = m_objects[0];
		skybox.m_models[0].m_transform.m_rotation.y += .0006f * delta;
		skybox.m_models[0].m_transform.m_rotation.x += .0006f * delta;
	}

	if(TInput::GetInstance()->IsKeyDown('Q'))
		Skybox.m_models[0].m_transform.m_rotation.z += 0.01f;
	if (TInput::GetInstance()->IsKeyDown('E'))
		Skybox.m_models[0].m_transform.m_rotation.z -= 0.01f;

	if (TInput::GetInstance()->IsKeyDown('Z'))
		Skybox.m_models[0].m_transform.m_rotation.x += 0.01f;
	if (TInput::GetInstance()->IsKeyDown('C'))
		Skybox.m_models[0].m_transform.m_rotation.x -= 0.01f;

	//CONSOLE_LOG(std::to_wstring(Skybox.m_models[0].m_transform.m_rotation.z) + L"   " + std::to_wstring(Skybox.m_models[0].m_transform.m_rotation.y));
}

void CBasicShapesScene::LoadScene(CRenderer* renderer)
{
	check(renderer);

	m_camera.SetStartingPosition(0, 5, -20);

	//Initialize debug lines
	m_debuglines.Initialize(renderer);

#if 0 //old way
	int32 SkyboxTexture = IO::TFileIO::LoadTextureFromFile(renderer, "SkyboxAssets", "Sepulchral.png");
	//IO::TFileIO::CreateCube(renderer, Skybox, SkyboxScale, SkyboxTexture);
#else
	m_skyboxCubeMap = IO::TFileIO::LoadTextureFromFile(renderer, "SkyboxAssets", "Sepulchral.png");
	IO::TFileIO::LoadAsset(renderer, "SkyboxAssets", "skybox.obj", Skybox);

	Skybox.m_models[0].m_rasterizationState = ERasterizerStates::eFillSolidCullFront;
#endif

	//______________teapots
	const int32 numteapots = 15;
	TObject teapots;

#if 1 //teapot
	IO::TFileIO::LoadAsset(renderer, "Teapot", "utah-teapot.obj", teapots);
#else
	IO::TFileIO::LoadAsset(renderer, "Shapes", "Sphere.obj", teapots);
#endif

	int32 i = 0, j = 0;
	float xPos = -10.f, zPos = 0.f;
	
	for (; i < 4; ++i, zPos += 6.f)
	{
		for (; j < 5; ++j, xPos += 6.f)
		{
			teapots.m_models[0].m_rasterizationState = ERasterizerStates::eFillSolidCullNone;
			TTransform transform = teapots.m_models[0].m_transform;
			transform.m_position.x = xPos;
			transform.m_position.z = zPos;
	
			teapots.m_models[0].m_meshes[0].m_instances.m_instanceTransforms.push_back(transform);

			TMaterial newMaterial = {};
			newMaterial.m_diffuseColor = renderer->GetMaterial(teapots.m_models[0].m_meshes[0].m_materialKey).m_diffuseColor;
			newMaterial.m_metallic = 1 - 0.25f * (float)i;
			newMaterial.m_roughness = 1 - 0.2f * (float)j;

			//CONSOLE_LOG(std::to_wstring(i) + L" " + std::to_wstring(j));

			teapots.m_models[0].m_meshes[0].m_instances.m_instanceMaterials.push_back(
				(int32)Algorithm::ArrPush_Back(renderer->GetMaterials(), newMaterial));
		}
	
		xPos = -10.f;
	
		j = 0;
	}

	m_objects.push_back(teapots);

#if 0 //floor
	TObject floor;
	IO::TFileIO::LoadAsset(renderer, "Shapes", "CheckeredFloor.obj", floor);
	floor.m_models[0].m_transform.m_position.z += 8.0f;
	floor.m_models[0].m_transform.m_position.y -= .5f;
	
	m_objects.push_back(floor);
#endif

	//______________lights
	TDirectionalLight Star;
	Star.m_position = float3(0.f, 100, -100);
	Star.m_color = TColors::White;
	Star.m_intensity = 0.8f;
	Star.m_lightType = ELightType::eDirectional;

	m_Lights.push_back(Star);
}

void CBasicShapesScene::UnloadScene()
{
	m_objects.clear();
}