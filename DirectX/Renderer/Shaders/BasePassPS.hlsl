#include "HLSLGlue.h"
#include "BasePassStructs.h"

START_CBUFFER(BasePassMaterial, b1)
float4 DiffuseColor;
END_CBUFFER(BasePassMaterial);

#if SHADER

Texture2D t_Diffuse;
SamplerState Sampler;

float4 MainPS(BasePassInPS input) : SV_TARGET
{
	float4 OutColor = 0;
	float4 diffuse = DiffuseColor;
	//diffuse = t_Diffuse.Sample(Sampler, input.uv);

	OutColor = diffuse;
	//return float4(input.uv, 0, 0);
	return DiffuseColor;
}
#endif