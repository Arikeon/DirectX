#if __cplusplus
#pragma once
#endif

#include "HLSLGlue.h"

START_CBUFFER(BasePassWVP, b0)
float4x4 World;
float4x4 View;
float4x4 Proj;
END_CBUFFER(BasePassWVP);

START_CBUFFER(BasePassMaterial, b1)

END_CBUFFER(BasePassMaterial);


struct BasePassInVS
{
	float3 position SEMANTIC(POSITION)
	float3 normal SEMANTIC(NORMAL)
	float3 tangents SEMANTIC(TANGENTS)
	float3 bitangents SEMANTIC(BITANGENTS)
	float2 texcoord SEMANTIC(TEXCOORD0)
};

struct BasePassInPS
{
	float4 svposition SEMANTIC(SV_POSITION)
	float3 position SEMANTIC(POSITION)
	float3 normal SEMANTIC(NORMAL)
	float2 uv SEMANTIC(TEXCOORD0)
};
