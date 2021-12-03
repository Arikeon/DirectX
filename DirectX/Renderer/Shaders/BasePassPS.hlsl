#include "HLSLGlue.h"
#include "StructCollection.h"

#if CPP || !USE_INSTANCING
START_CBUFFER(BasePassMaterial, b0)
float3 DiffuseColor;
//float3 Specular;
float Roughness;
float Metallic;
END_CBUFFER(BasePassMaterial);
#endif

#if CPP || USE_INSTANCING
START_CBUFFER(InstancedBasePassMaterial, b0)
float4	InstancedDiffuseColor[MAX_INSTANCE];
float4	InstancedSpecular[MAX_INSTANCE];
float	InstancedRoughness[MAX_INSTANCE];
float	InstancedMetallic[MAX_INSTANCE];
END_CBUFFER(InstancedBasePassMaterial);
#endif

#if SHADER

struct GBufferOut
{
	float4 color						SEMANTIC(SV_TARGET0);
#if USE_GBUFFER
	float4 worldnormal					SEMANTIC(SV_TARGET1);
	float4 roughnessmetallicspecular	SEMANTIC(SV_TARGET2);
#endif
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

	#if USE_INSTANCING
		output.color		= InstancedDiffuseColor[input.instanceid];
	#else
		output.color		= float4(DiffuseColor, 1.0f);
	#endif //USE_INSTANCING

#endif //USE_TEXTURE_DIFFUSE

#if USE_GBUFFER
	#if USE_TEXTURE_NORMAL
			output.worldnormal	= t_Normal.Sample(s_Sampler, input.uv);
	#else
			output.worldnormal	= float4(input.normal, 0.f);
	#endif //USE_TEXTURE_NORMAL
#endif


#if USE_GBUFFER
	#if USE_INSTANCING
		output.roughnessmetallicspecular.x		= InstancedRoughness[input.instanceid];
		output.roughnessmetallicspecular.y		= InstancedMetallic[input.instanceid];
	#else
		output.roughnessmetallicspecular.x		= Roughness;
		output.roughnessmetallicspecular.y		= Metallic;
	#endif  //USE_INSTANCING
		//output.roughnessmetallicspecular.z	= Specular;
#endif

	return output;
}
#endif