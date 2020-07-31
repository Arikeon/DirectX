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

	verticies[0].position = float3(1,-1,-1);
	verticies[1].position = float3(1,-1,1);
	verticies[2].position = float3(-1,-1,1);
	verticies[3].position = float3(-1,-1,-1);
	verticies[4].position = float3(1,1,-1);
	verticies[5].position = float3(1,1,1);
	verticies[6].position = float3(-1,1,1);
	verticies[7].position = float3(-1,1,-1);

	std::vector<unsigned int> indicies;
	indicies.resize(36);
	indicies[0] = 1;
	indicies[1] = 2;
	indicies[2] = 3;
	indicies[3] = 7;
	indicies[4] = 6;
	indicies[5] = 5;
	indicies[6] = 4;
	indicies[7] = 5;
	indicies[8] = 1;
	indicies[9] = 5;
	indicies[10] = 6;
	indicies[11] = 2;
	indicies[12] = 2;
	indicies[13] = 6;
	indicies[14] = 7;
	indicies[15] = 0;
	indicies[16] = 3;
	indicies[17] = 7;
	indicies[18] = 0;
	indicies[19] = 1;
	indicies[20] = 3;
	indicies[21] = 4;
	indicies[22] = 7;
	indicies[23] = 5;
	indicies[24] = 0;
	indicies[25] = 4;
	indicies[26] = 1;
	indicies[27] = 1;
	indicies[28] = 5;
	indicies[29] = 2;
	indicies[30] = 3;
	indicies[31] = 2;
	indicies[32] = 7;
	indicies[33] = 4;
	indicies[34] = 0;
	indicies[35] = 8;

	TModel BasicCube;
	BasicCube.m_name = "BasicCube";
	BasicCube.m_mesh.CreateMesh<BasePassInVS>(renderer, verticies, indicies);
	m_models.push_back(BasicCube);
}

void CBasicShapesScene::UnloadScene()
{
	m_models.clear();
}