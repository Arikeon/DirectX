#include "HLSLGlue.h"
#include "StructCollection.h"

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
	output.svposition = mul(InstanceMatrix[instanceid], output.svposition);
	output.instanceid = instanceid;
#else
	output.svposition = mul(WorldViewProjection, output.svposition);
#endif

	return output;
}

#endif
