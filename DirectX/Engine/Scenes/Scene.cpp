#include "Scene.h"
#include "Renderer.h"
#include <array>
#include "BasePass.h"
#include "ScreenQuadStructs.h"

//Passes
#include "Pass.h"
#include "BasePass.h"
#include "DeferredLightingPass.h"


CScene::~CScene()
{
}

void CScene::Initialize(CRenderer* renderer)
{
	InitializeScreenQuad(renderer);
}

void CScene::InitializeScreenQuad(CRenderer* renderer)
{
	const float size = 1.0f;

	std::vector<uint32> indicies;
	indicies.resize(6);
	indicies[0] = 0;
	indicies[1] = 1;
	indicies[2] = 2;

	indicies[3] = 0;
	indicies[4] = 2;
	indicies[5] = 3;


	/**
	1	2
	0	3
	**/

	std::vector<ScreenQuadInVS> verticies;
	verticies.resize(4);
	verticies[0] = ScreenQuadInVS(float3(-size, -size, 0.f), float2(0.f, 1.0f));
	verticies[1] = ScreenQuadInVS(float3(-size, size, 0.f), float2(0.f, 0.0f));
	verticies[2] = ScreenQuadInVS(float3(size, size, 0.f), float2(1.f, 0.0f));
	verticies[3] = ScreenQuadInVS(float3(size, -size, 0.f), float2(1.f, 1.0f));

	TModel ScreenQuadModel = {};
	ScreenQuadModel.m_name = "ScreenQuadModel";

	TMesh newMesh = {};
	newMesh.CreateMesh<ScreenQuadInVS>(renderer, verticies, indicies);
	check(newMesh.m_vertexkey != -1 && newMesh.m_indexkey != -1);

	ScreenQuadModel.m_meshes.push_back(newMesh);
	m_ScreenQuad.m_models.push_back(ScreenQuadModel);
}

void CScene::DrawTransform(TModel& model)
{
	matrix LocalView = model.m_transform.GetMatrix<true, true, false>();
	vector position = LocalView.r[3];
	LocalView.r[3] = {};

	const float LineSize = 3.0f;

	vector right	= XMVectorSet(LineSize, 0, 0, 0);
	vector up		= XMVectorSet(0, LineSize, 0, 0);
	vector forward	= XMVectorSet(0, 0, LineSize, 0);
	right			= position + XMVector3Transform(right, LocalView);
	up				= position + XMVector3Transform(up, LocalView);
	forward			= position + XMVector3Transform(forward, LocalView);

	float3 fposition;
	float3 fright;
	float3 fup;
	float3 fforward;

	XMStoreFloat3(&fposition, position);
	XMStoreFloat3(&fright, right);
	XMStoreFloat3(&fup, up);
	XMStoreFloat3(&fforward, forward);

	//m_debuglines.AddDebugLines(fposition, fright, TColors::Red);
	//m_debuglines.AddDebugLines(fposition, fup, TColors::Green);
	//m_debuglines.AddDebugLines(fposition, fforward, TColors::Blue);
}

void CScene::DrawModelTransforms()
{
	if (m_bDrawTransforms)
	{
		for (int i = 0; i < m_objects.size(); ++i)
		{
			TObject& object = m_objects[i];
			for (int j = 0; j < object.m_models.size(); ++j)
			{
				TModel& model = object.m_models[j];
				DrawTransform(model);
			}
		}
	}
}

void CScene::RenderScene(CRenderer* renderer, TWindow window, float delta)
{
	static TBasePass* basePass = nullptr;
	static TDeferredLightingPass* deferredLightingPass = nullptr;

	bool bSingletonInitialized = false;
	if (!bSingletonInitialized)
	{
		bSingletonInitialized = true;
		basePass = new TBasePass;
		deferredLightingPass = new TDeferredLightingPass;
	}

	m_camera.Update(delta);
	DrawModelTransforms();

	basePass->Render(renderer, GetObjects(), m_debuglines, m_camera, window,delta);
	deferredLightingPass->Render(renderer, GetLights(), m_ScreenQuad, m_camera, delta);
}