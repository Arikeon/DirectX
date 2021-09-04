#include "HLSLGlue.h"
#include "StructCollection.h"

START_CBUFFER(BasePassMaterial, b0)
float4 DiffuseColor;
float Roughness;
float Metallic;
END_CBUFFER(BasePassMaterial);

#if SHADER

struct GBufferOut
{
	float4 color						SEMANTIC(SV_TARGET0);
	float4 worldnormal					SEMANTIC(SV_TARGET1);
	float4 roughnessmetallicspecular	SEMANTIC(SV_TARGET2);
};

SamplerState s_Sampler;
Texture2D t_Diffuse;
Texture2D t_Normal;

GBufferOut MainPS(BasePassInPS input)
{
	GBufferOut output;
#if USE_TEXTURE_DIFFUSE
		output.color		= t_Diffuse.Sample(s_Sampler, input.uv);
#else
		output.color		= DiffuseColor;
#endif //USE_TEXTURE_DIFFUSE

#if USE_TEXTURE_NORMAL
		output.worldnormal	= t_Normal.Sample(s_Sampler, input.uv);
#else
		output.worldnormal	= float4(input.normal, 0.f);
#endif //USE_TEXTURE_NORMAL
	output.roughnessmetallicdistance.x		= Roughness;
	output.roughnessmetallicdistance.y		= Metallic;

	return output;
}
#endif