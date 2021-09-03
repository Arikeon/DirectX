#include "HLSLGlue.h"
#include "BasePassStructs.h"
#if SHADER
#pragma pack_matrix(row_major)
#endif

#define MAX_INSTANCE 20 //permute?

START_CBUFFER(BasePassWVP, b0)
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
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
#else
	output.svposition = mul(output.svposition, World);
#endif
	output.svposition = mul(output.svposition, View);

	output.worldviewPos = output.svposition.xyz;

	output.svposition = mul(output.svposition, Proj);
#if USE_INSTANCING
	output.instanceid = instanceid;
#endif

	return output;
}

#endif
