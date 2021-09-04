#include "HLSLGlue.h"
#include "StructCollection.h"

float4 MainPS(DebugLinesInPS input) : SV_TARGET
{
	return float4(input.color, 0.f);
}