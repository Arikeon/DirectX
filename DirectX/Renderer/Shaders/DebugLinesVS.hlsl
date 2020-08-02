#pragma pack_matrix(row_major)
#include "HLSLGlue.h"
#include "DebugLinesStructs.h"

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