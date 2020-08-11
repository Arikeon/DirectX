#include "HLSLGlue.h"
#include "BasePassStructs.h"

Texture2D t_Diffuse;
SamplerState Sampler;

float4 MainPS(BasePassInPS input) : SV_TARGET
{
	float4 diffuse = t_Diffuse.Sample(Sampler, input.uv);

	if (diffuse.x == 0)
		diffuse.x = 1;
	return diffuse;
}