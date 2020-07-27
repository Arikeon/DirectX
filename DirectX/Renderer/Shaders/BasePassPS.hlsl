#include "HLSLGlue.h"
#include "BasePassStructs.h"

float4 MainPS(BasePassInPS input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}