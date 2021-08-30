#include "HLSLGlue.h"
#include "ScreenQuadStructs.h"

START_CBUFFER(LightBuffer, b0)
float4 positionAndType;
END_CBUFFER(LightBuffer);

#if SHADER

SamplerState s_Sampler;
Texture2D t_Diffuse;
Texture2D t_WorldNormal;
Texture2D t_Roughness;
Texture2D t_Metallic;

float4 MainPS(ScreenQuadInPS input) : SV_TARGET0
{
	float4 color = t_Diffuse.Sample(s_Sampler, input.uv);
	float4 worldnormal = t_WorldNormal.Sample(s_Sampler, input.uv);
	float roughness = t_Roughness.Sample(s_Sampler, input.uv);
	float metallic = t_Metallic.Sample(s_Sampler, input.uv);

	return color;
}

#endif