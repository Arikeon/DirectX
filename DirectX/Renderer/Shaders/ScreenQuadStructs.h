#include "HLSLGlue.h"

#if CPP
#pragma once
#endif

struct ScreenQuadInVS
{
	float3 position SEMANTIC(POSITION)
	float2 texcoord SEMANTIC(TEXCOORD0)
};

struct ScreenQuadInPS
{
	CENTROID float4 svposition SEMANTIC(SV_POSITION)
	CENTROID NOPERSPECTIVE float2 uv SEMANTIC(TEXCOORD0)
};