#pragma once
#include "Pass.h"
#include "Camera.h"
#include "Window.h"
#include "D3D11Interface.h"

struct TBasePass : public TPass
{
	TBasePass() {}
	~TBasePass() {}

	void Setup(CRenderer* renderer, float delta);
	
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