#include "Scene.h"
#include "Renderer.h"
#include <array>

void CScene::InitializePasses(CRenderer* renderer)
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

	m_debuglines.AddDebugLines(fposition, fright, TColors::Red);
	m_debuglines.AddDebugLines(fposition, fup, TColors::Green);
	m_debuglines.AddDebugLines(fposition, fforward, TColors::Blue);
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
	renderer->ResetPipeline();
	m_camera.Update(delta);
	DrawModelTransforms();
	m_basepass.Render(renderer, GetObjects(), m_debuglines, m_camera, window,delta);
}