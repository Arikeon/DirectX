#include "HLSLGlue.h"
#include "BasePassStructs.h"

START_CBUFFER(BasePassMaterial, b0)
float4 DiffuseColor;
float Roughness;
float Metallic;
int bHasDiffuse; //hacky //TODO
int bHasNormal; //hacky //TODO
END_CBUFFER(BasePassMaterial);

#if SHADER

struct GBufferOut
{
	float4 color		SEMANTIC(SV_TARGET0)
	float4 worldnormal	SEMANTIC(SV_TARGET1)
	float4 roughness	SEMANTIC(SV_TARGET2)
	float4 metallic		SEMANTIC(SV_TARGET3)
};

SamplerState Sampler;
Texture2D t_Diffuse;
Texture2D t_Normal;

GBufferOut MainPS(BasePassInPS input)
{
	GBufferOut output;
	if (bHasDiffuse)
		output.color		= t_Diffuse.Sample(Sampler, input.uv);
	else
		output.color		= DiffuseColor;

	if (bHasNormal)
		output.worldnormal	= t_Normal.Sample(Sampler, input.uv);
	else
		output.worldnormal	= float4(input.normal, 0.f);
	output.roughness	= Roughness;
	output.metallic		= Metallic;
	return output;
}
#endif