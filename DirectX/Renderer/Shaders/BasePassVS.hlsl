#include "HLSLGlue.h"
#include "StructCollection.h"

#if CPP || !USE_INSTANCING
START_CBUFFER(BasePassWVP, b0)
float4x4 WorldViewProjection;
END_CBUFFER(BasePassWVP);
#endif

#if CPP || USE_INSTANCING
START_CBUFFER(InstancedBasePassWVP, b0)
float4x4 InstancedWorldViewProjection[MAX_INSTANCE];
END_CBUFFER(InstancedBasePassWVP);
#endif


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
	output.svposition = mul(InstancedWorldViewProjection[instanceid], output.svposition);
	output.instanceid = instanceid;
#else
	output.svposition = mul(WorldViewProjection, output.svposition);
#endif

	return output;
}

#endif
