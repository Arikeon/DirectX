#include "HLSLGlue.h"
#include "ScreenQuadStructs.h"

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