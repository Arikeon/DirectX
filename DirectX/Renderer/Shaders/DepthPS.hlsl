#include "HLSLGlue.h"
#include "StructCollection.h"

#if SHADER

float4 MainPS(DepthPassInPS input) : SV_TARGET
{
	return float4(0.f, 0.f, 0.f, 0.f);
}
#endif