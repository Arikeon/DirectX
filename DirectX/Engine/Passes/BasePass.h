#pragma once
#include "Pass.h"

struct TBasePass : public TPass
{
	void DrawMeshes(CRenderer* renderer,
		TShader shader,
		CCamera camera,
		TWindow window,
		float delta,
		TModel model,
		std::vector<ID3D11RenderTargetView*> rtvs
	);
	
	void RenderSkyBoxAndUI(CRenderer* renderer,
		TObject& skybox,
		TDebugLines debuglines,
		CCamera camera,
		TWindow window,
		float delta);

	void RenderSceneMesh(CRenderer* renderer,
		std::vector<TObject>& objects,
		CCamera camera,
		TWindow window,
		float delta);
};