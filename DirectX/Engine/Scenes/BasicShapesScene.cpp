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
}

void CBasicShapesScene::LoadScene(CRenderer* renderer)
{
	check(renderer);

	m_camera.SetStartingPosition(0, 0, -20);

	//Initialize debug lines
	m_debuglines.Initialize(renderer);

	//______________skybox
	//https://wwwtyro.github.io/space-3d/
	float SkyboxScale = 9999.f;
	int32 SkyboxTexture = IO::TFileIO::LoadTextureFromFile(renderer, "SkyboxAssets", "skybox.png");
	IO::TFileIO::CreateCube(renderer, Skybox, SkyboxScale, SkyboxTexture);
	Skybox.m_models[0].m_rasterizationState = ERasterizerStates::eFillSolidCullFront;

	//______________teapots
	const int32 numteapots = 15;
	TObject teapots;
	//slow
	IO::TFileIO::LoadAsset(renderer, "Teapot", "utah-teapot.obj", teapots);
	//IO::TFileIO::LoadAsset(renderer, "Shapes", "cube.obj", teapots);

	int32 i = 0, j = 0;
	float xPos = -10.f, zPos = 0.f;
	
	for (; i < 4; ++i, zPos += 6.f)
	{
		for (; j < 5; ++j, xPos += 6.f)
		{
			TTransform transform = teapots.m_models[0].m_transform;
			transform.m_position.x = xPos;
			transform.m_position.z = zPos;
	
			teapots.m_models[0].m_meshes[0].m_instances.m_instanceTransforms.push_back(transform);

			TMaterial newMaterial = {};
			newMaterial.m_diffuseColor = renderer->GetMaterial(teapots.m_models[0].m_meshes[0].m_materialKey).m_diffuseColor;
			newMaterial.m_metallic = 0.33f * (float)i;
			newMaterial.m_roughness = 0.2f * (float)j;

			teapots.m_models[0].m_meshes[0].m_instances.m_instanceMaterials.push_back(
				(int32)Algorithm::ArrPush_Back(renderer->GetMaterials(), newMaterial));
		}
	
		xPos = -10.f;
	
		j = 0;
	}

	m_objects.push_back(teapots);

	//______________lights
	TDirectionalLight Star;
	Star.m_position = float3(0.f, 10000, -10000);
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