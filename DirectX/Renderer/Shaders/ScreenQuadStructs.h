#include "HLSLGlue.h"

#if CPP
#pragma once
#endif

struct ScreenQuadInVS
{
#if CPP
	ScreenQuadInVS() = default;
	ScreenQuadInVS(float3 pos, float2 uv) : position(pos), uv(uv) {}
#endif
	float3 position SEMANTIC(POSITION);
	float2 uv SEMANTIC(TEXCOORD0);
};

struct ScreenQuadInPS
{
	CENTROID float4 svposition SEMANTIC(SV_POSITION);
	CENTROID NOPERSPECTIVE float2 uv SEMANTIC(TEXCOORD0);
};