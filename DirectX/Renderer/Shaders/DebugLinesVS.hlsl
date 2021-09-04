#include "HLSLGlue.h"
#include "StructCollection.h"

START_CBUFFER(DebugLinesWVP, b0)
float4x4 WorldViewProjection;
END_CBUFFER(DebugLinesWVP);

#if SHADER
DebugLinesInPS MainVS(DebugLinesInVS input)
{
	DebugLinesInPS output;
	output.color = input.color;

	output.position = input.position;
	output.svposition = float4(output.position, 1.0f);
	output.svposition = mul(WorldViewProjection, output.svposition);
	return output;
}
#endif