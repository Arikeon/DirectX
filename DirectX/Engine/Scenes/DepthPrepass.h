#pragma once

#include "Pass.h"


struct TDepthPrePass : public TPass
{
	void DrawMeshes(CRenderer* renderer,
		TShader shader,
		CCamera camera,
		TWindow window,
		float delta,
		TModel model
	);

	void Render(CRenderer* renderer,
		std::vector<TObject>& objects,
		TDebugLines debuglines,
		CCamera camera,
		TWindow window,
		float delta);
};