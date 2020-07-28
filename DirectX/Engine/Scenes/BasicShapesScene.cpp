#include "BasicShapesScene.h"
#include "Renderer.h"
#include "StructCollection.h"
#include "Model.h"
#include "Mesh.h"

CBasicShapesScene::CBasicShapesScene()
{
	m_selectedscene = EScene::eBasicShapesScene;
}


CBasicShapesScene::~CBasicShapesScene()
{

}

void CBasicShapesScene::LoadScene(CRenderer* renderer)
{
	check(renderer);

	std::vector<BasePassInVS> verticies;
	verticies.resize(8);

	verticies[0].position = float3(-.5f, -.5f, .5f);
	verticies[1].position = float3(.5f, .5f, .5f);
	verticies[2].position = float3(-.5f, .5f, .5f);
	verticies[3].position = float3(.5f, .5f, .5f);
	verticies[4].position = float3(-.5f, .5f, -.5f);
	verticies[5].position = float3(.5f, .5f, -.5f);
	verticies[6].position = float3(-.5f, -.5f, -.5f);
	verticies[7].position = float3(.5f, -.5f, -.5f);

	std::vector<unsigned int> indicies;
	indicies.resize(0);


	TModel BasicCube;
	BasicCube.m_name = "BasicCube";
	BasicCube.m_mesh.CreateMesh<BasePassInVS>(renderer, verticies, indicies);
}

void CBasicShapesScene::UnloadScene()
{
	m_models.clear();
}