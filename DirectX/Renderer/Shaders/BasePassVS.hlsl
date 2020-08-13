#include "HLSLGlue.h"
#include "BasePassStructs.h"

START_CBUFFER(BasePassWVP, b0)
float4x4 World;
float4x4 View;
float4x4 Proj;
END_CBUFFER(BasePassWVP);

#if SHADER

#pragma pack_matrix(row_major)
BasePassInPS MainVS(BasePassInVS input)
{
	BasePassInPS output;
	output.normal = input.normal;
	output.uv = input.texcoord;

	output.position = input.position;
	output.svposition = float4(output.position, 1.0f);
	output.svposition = mul(output.svposition, World);
	output.svposition = mul(output.svposition, View);
	output.svposition = mul(output.svposition, Proj);
	return output;
}

#endif
