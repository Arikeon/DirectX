#if __cplusplus
#pragma once
#endif

#include "HLSLGlue.h"

START_CBUFFER(DebugLinesWVP, b0)
float4x4 World;
float4x4 View;
float4x4 Proj;
END_CBUFFER(DebugLinesWVP);

struct DebugLinesInVS
{
	float3 position SEMANTIC(POSITION)
	float3 color SEMANTIC(COLOR)
};

struct DebugLinesInPS
{
	float4 svposition SEMANTIC(SV_POSITION)
	float3 position SEMANTIC(POSITION)
	float3 color SEMANTIC(COLOR)
};

