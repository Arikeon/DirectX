#include "HLSLGlue.h"
#include "DebugLinesStructs.h"

#if SHADER
#pragma pack_matrix(row_major)
#endif
START_CBUFFER(DebugLinesWVP, b0)
float4x4 World;
float4x4 View;
float4x4 Proj;
END_CBUFFER(DebugLinesWVP);

#if SHADER
DebugLinesInPS MainVS(DebugLinesInVS input)
{
	DebugLinesInPS output;
	output.color = input.color;

	output.position = input.position;
	output.svposition = float4(output.position, 1.0f);
	output.svposition = mul(output.svposition, World);
	output.svposition = mul(output.svposition, View);
	output.svposition = mul(output.svposition, Proj);

	return output;
}
#endif