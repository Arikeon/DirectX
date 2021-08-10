#pragma once
#include "Pass.h"

struct TDeferredLightingPass : public TPass
{
	void Render(CRenderer* renderer, TObject& ScreenQuadObject, float delta);

private:
	void DrawScreenQuad(CRenderer* renderer,
		TShader DeferredLightingPS,
		TShader ScreenQuadVS,
		TModel& ScreenQuadModel,
		float delta);
};