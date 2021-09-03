#include "HLSLGlue.h"
#include "ScreenQuadStructs.h"

START_CBUFFER(ScreenQuadPassVP, b0)
float4x4 WorldViewProj;
END_CBUFFER(ScreenQuadPassVP);

#if SHADER

ScreenQuadInPS MainVS( ScreenQuadInVS input )
{
	ScreenQuadInPS output;
	
	float4 position = float4(input.position, 1.f);

	output.svposition = position;
	output.uv = input.uv;

	return output;
}

#endif