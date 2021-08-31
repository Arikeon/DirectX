#include "HLSLGlue.h"

#if CPP
#pragma once
#endif

struct DebugLinesInVS
{
	float3 position SEMANTIC(POSITION);
	float3 color SEMANTIC(COLOR);
};

struct DebugLinesInPS
{
	float4 svposition SEMANTIC(SV_POSITION);
	float3 position SEMANTIC(POSITION);
	float3 color SEMANTIC(COLOR);
};

