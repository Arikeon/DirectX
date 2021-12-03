#pragma once
#include "Pass.h"

struct TDeferredLightingPass : public TPass
{
	void Render(CRenderer* renderer,
		std::vector<TLight> Lights,
		CCamera camera,
		TWindow window,
		float delta);

private:
	void DrawScreenQuad(CRenderer* renderer,
		std::vector<TLight> Lights,
		TShader DeferredLightingPS,
		TShader ScreenQuadVS,

		CCamera camera,
		TWindow window,
		float delta);
};