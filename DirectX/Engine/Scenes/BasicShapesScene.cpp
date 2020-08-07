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


}

void CBasicShapesScene::UnloadScene()
{
	m_objects.clear();
}