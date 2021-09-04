#include "HLSLGlue.h"
#include "StructCollection.h"


START_CBUFFER(DepthPassWVP, b0)
float4x4 WorldViewProjection;
END_CBUFFER(DepthPassWVP);

START_CBUFFER(DepthPassInstanceBuffer, b1)
#if CPP || USE_INSTANCING
	float4x4 InstanceMatrix[MAX_INSTANCE];
#endif
END_CBUFFER(DepthPassInstanceBuffer);

#if SHADER

DepthPassInPS MainVS(DepthPassInVS input
#if USE_INSTANCING
, uint instanceid SEMANTIC(SV_InstanceID)
#endif
)
{
	DepthPassInPS output;
	output.svposition = float4(input.position, 1.0f);

#if USE_INSTANCING
	output.svposition = mul(InstanceMatrix[instanceid], output.svposition );
#else
	output.svposition = mul(WorldViewProjection, output.svposition );
#endif

	return output;
}

#endif