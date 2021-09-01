#pragma once
#include "Pass.h"

struct TDeferredLightingPass : public TPass
{
	void Render(CRenderer* renderer,
		std::vector<TLight> Lights,
		TObject& ScreenQuadObject,
		CCamera camera,
		float delta);

private:
	void DrawScreenQuad(CRenderer* renderer,
		std::vector<TLight> Lights,
		TShader DeferredLightingPS,
		TShader ScreenQuadVS,
		TModel& ScreenQuadModel,
		CCamera camera,
		float delta);
};