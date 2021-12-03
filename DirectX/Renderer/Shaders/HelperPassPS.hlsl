#include "HLSLGlue.h"
#include "StructCollection.h"

#if SHADER
SamplerState s_Sampler;
Texture2D t_Target : register(t0);

float4 MainPS(ScreenQuadInPS input) : SV_TARGET0
{
	float4 color = t_Target.Sample(s_Sampler, input.uv);
	return color;
}
#endif