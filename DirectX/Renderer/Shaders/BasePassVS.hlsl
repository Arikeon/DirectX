#include "HLSLGlue.h"
#include "BasePassStructs.h"

BasePassInPS MainVS(BasePassInVS input )
{
	BasePassInPS output;
	output.texcoord = input.texcoord;

	output.position = input.position;
	output.svposition = float4(output.position, 1.0f);
	output.svposition = mul(output.svposition, World);
	output.svposition = mul(output.svposition, View);
	output.svposition = mul(output.svposition, Proj);
	return output;
}