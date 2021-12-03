#include "Scene.h"
#include "Renderer.h"
#include <array>
#include "BasePass.h"
#include "StructCollection.h"

//Passes
#include "Pass.h"
#include "DepthPrepass.h"
#include "BasePass.h"
#include "DeferredLightingPass.h"
#include "HelperPass.h"

CScene::~CScene()
{
}

void CScene::Initialize(CRenderer* renderer)
{
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
	static TDepthPrePass* depthprepass = nullptr;
	static THelperPass* helperpass = nullptr;

	//This is a hack to some weird compile error TODO static class?
	bool bSingletonInitialized = false;
	if (!bSingletonInitialized)
	{
		TPass::Initialize(renderer);
		bSingletonInitialized = true;
		basePass = new TBasePass;
		deferredLightingPass = new TDeferredLightingPass;
		depthprepass = new TDepthPrePass;
		helperpass = new THelperPass;
	}

	m_camera.Update(delta);
	DrawModelTransforms();

	renderer->ConstructFrameBuffer(window, m_camera);

	depthprepass->Render(renderer, GetObjects(), m_debuglines, m_camera, window, delta);
	basePass->RenderSceneMesh(renderer, GetObjects(), m_camera, window,delta);
	deferredLightingPass->Render(renderer, GetLights(), m_camera, window, delta);
	basePass->RenderSkyBoxAndUI(renderer, Skybox, m_debuglines, m_camera, window,delta);

	helperpass->CopyBuffer(renderer, renderer->GetRenderTarget(ERenderTargetKey::eColor), renderer->GetRenderTarget(ERenderTargetKey::eBackBufeer));
	renderer->GetFrameBuffer().bIsUpToDate = false;
}