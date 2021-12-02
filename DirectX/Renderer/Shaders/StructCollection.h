#include "HLSLGlue.h"
#include "FrameBuffer.h"
#if CPP
#pragma once
#endif

#ifndef MAX_INSTANCE
#define MAX_INSTANCE 20
#endif


//____________________________Base Pass____________________________

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
	float3 normal SEMANTIC(NORMAL);
	float2 uv SEMANTIC(TEXCOORD0);
	NOINTERPOLATION int	   instanceid SEMANTIC(INDEX);
};

//____________________________Depth Pass____________________________

struct DepthPassInVS
{
	float3 position SEMANTIC(POSITION);
};

struct DepthPassInPS
{
	float4 svposition SEMANTIC(SV_POSITION);
};

//____________________________Debug Lines Pass____________________________

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

//____________________________Screen Quad Pass____________________________

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