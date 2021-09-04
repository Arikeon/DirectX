#include "HLSLGlue.h"
#include "BasePassStructs.h"
#if SHADER
#pragma pack_matrix(row_major)
#endif

#define MAX_INSTANCE 20 //permute?

START_CBUFFER(BasePassWVP, b0)
	float4x4 WorldViewProjection;
END_CBUFFER(BasePassWVP);

START_CBUFFER(BasePassInstanceBuffer, b1)
#if CPP || USE_INSTANCING
	float4x4 InstanceMatrix[MAX_INSTANCE];
#endif
END_CBUFFER(BasePassInstanceBuffer);

#if SHADER

BasePassInPS MainVS(BasePassInVS input
#if USE_INSTANCING
, uint instanceid SEMANTIC(SV_InstanceID)
#endif
)
{
	BasePassInPS output;
	output.normal = input.normal;
	output.uv = input.texcoord;

	output.svposition = float4(input.position, 1.0f);
#if USE_INSTANCING
	output.svposition = mul(output.svposition, InstanceMatrix[instanceid]);
	output.instanceid = instanceid;
#else
	output.svposition = mul(output.svposition, WorldViewProjection);
#endif

	return output;
}

#endif
