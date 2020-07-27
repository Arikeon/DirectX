#include "HLSLGlue.h"
#include "BasePassStructs.h"

BasePassInPS MainVS(BasePassInVS input )
{
	BasePassInPS output;

	output.position = mul(float4(input.position, 0), World);
	output.svposition = mul(output.position, World);
	output.svposition = mul(output.svposition, View);
	output.svposition = mul(output.svposition, Proj);

	output.texcoord = input.texcoord;

	return output;
}