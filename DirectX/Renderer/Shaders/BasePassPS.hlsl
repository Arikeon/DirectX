#include "HLSLGlue.h"
#include "BasePassStructs.h"

START_CBUFFER(BasePassMaterial, b0)
float4 DiffuseColor;
END_CBUFFER(BasePassMaterial);

#if SHADER

Texture2D t_Diffuse : register(t0);
SamplerState Sampler : register(s0);

float4 MainPS(BasePassInPS input) : SV_TARGET
{
	float4 OutColor = 0;
	float4 diffuse = DiffuseColor;
	diffuse = t_Diffuse.Sample(Sampler, input.uv);

	OutColor = diffuse;
	//return float4(input.uv, 0, 0);
	return OutColor;
}
#endif