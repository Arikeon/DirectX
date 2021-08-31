#include "HLSLGlue.h"

#if CPP
#pragma once
#endif

struct BasePassInVS
{
	float3 position SEMANTIC(POSITION);
	float3 normal SEMANTIC(NORMAL);
	float3 tangents SEMANTIC(TANGENTS);
	float3 bitangents SEMANTIC(BITANGENTS);
	float2 texcoord SEMANTIC(TEXCOORD0);
};

struct BasePassInPS
{
	float4 svposition SEMANTIC(SV_POSITION);
	float3 position SEMANTIC(POSITION);
	float3 normal SEMANTIC(NORMAL);
	float2 uv SEMANTIC(TEXCOORD0);
	int	   instanceid SEMANTIC(INDEX);
};