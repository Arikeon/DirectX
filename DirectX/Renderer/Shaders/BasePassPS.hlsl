#include "HLSLGlue.h"
#include "BasePassStructs.h"

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
	float4 roughnessmetallicdistance	SEMANTIC(SV_TARGET2);
};

SamplerState Sampler;
Texture2D t_Diffuse;
Texture2D t_Normal;

GBufferOut MainPS(BasePassInPS input)
{
	GBufferOut output;
#if USE_TEXTURE_DIFFUSE
		output.color		= t_Diffuse.Sample(Sampler, input.uv);
#else
		output.color		= DiffuseColor;
#endif //USE_TEXTURE_DIFFUSE

#if USE_TEXTURE_NORMAL
		output.worldnormal	= t_Normal.Sample(Sampler, input.uv);
#else
		output.worldnormal	= float4(input.normal, 0.f);
#endif //USE_TEXTURE_NORMAL
	output.roughnessmetallicdistance.x		= Roughness;
	output.roughnessmetallicdistance.y		= Metallic;
	output.roughnessmetallicdistance.z		= length(input.worldviewPos);

	return output;
}
#endif