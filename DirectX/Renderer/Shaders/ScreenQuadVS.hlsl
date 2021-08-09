#include "HLSLGlue.h"
#include "ScreenQuadStructs.h"

#if SHADER

ScreenQuadInPS MainVS( ScreenQuadInVS input )
{
	ScreenQuadInPS output;
	output.svposition = float4(input.position, 0.f);
	output.uv = input.texcoord;
	return output;
}

#endif