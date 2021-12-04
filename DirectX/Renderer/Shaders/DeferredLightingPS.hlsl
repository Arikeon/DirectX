#include "HLSLGlue.h"
#include "StructCollection.h"
#include "FrameBuffer.h"

//TODO 2.5d culling?
#define MAX_NUM_LIGHTS 30

struct LightData
{
	float4 LColor;
	float4 LPositionAndIntensity;
};

START_CBUFFER(LightBuffer, b1)
LightData LData[MAX_NUM_LIGHTS];
float AmbientStrength;
int DirectionalLightCount;
int PointLightCount;
int SpotLightCount;
END_CBUFFER(LightBuffer);

#if SHADER
#include "BRDF.hlsl"

SamplerState s_Sampler;
Texture2D t_Diffuse						: register(t0);
Texture2D t_Depth						: register(t1);
Texture2D t_WorldNormal					: register(t2);
Texture2D t_RoughnessMetallicSpecular	: register(t3);

//Texture2D t_DirectionalShadow;

float3 GetPositionVS(float2 uv, float depth)
{
	//https://mynameismjp.wordpress.com/2009/03/10/reconstructing-position-from-depth/
	float x = uv.x * 2.f - 1.f;
	float y = (1.f - uv.y) * 2.f - 1.f;
	float4 projectedPosition = float4(x, y, depth, 1.f);
	float4 viewPosition = mul(InverseProjection, projectedPosition);
	return viewPosition.xyz / viewPosition.w;
}

float3 ComputeDirectionalLight(LightData DirD, GBufferData BufferD)
{
	float radiance = DirD.LColor * DirD.LPositionAndIntensity.w;
	return PhongShade(DirD, BufferD) * radiance;
}

float4 MainPS(ScreenQuadInPS input) : SV_TARGET0
{
	float4 diffuse = t_Diffuse.Sample(s_Sampler, input.uv);
	float4 worldNormal = t_WorldNormal.Sample(s_Sampler, input.uv);

	float distance = t_Depth.Sample(s_Sampler, input.uv).r;
	float3 worldPos = GetPositionVS(input.uv, distance);
	worldPos = mul(InverseView, worldPos) + CameraPosition;

	float roughness = t_RoughnessMetallicSpecular.Sample(s_Sampler, input.uv).x;
	float metallic = t_RoughnessMetallicSpecular.Sample(s_Sampler, input.uv).y;
	const float SpecularIntensity = 1.0f;

	float4 color = float4(diffuse.xyz * AmbientStrength, 1.0f);

	GBufferData BufferD;
	BufferD.Diffuse		= diffuse;
	BufferD.N			= worldNormal;
	BufferD.P			= worldPos;
	BufferD.roughness	= roughness;
	BufferD.metallic	= metallic;
	BufferD.specular	= SpecularIntensity;

#if LIGHT_TYPE_DIRECTIONAL
	LightData LightD = LData[0];
	color.xyz += ComputeDirectionalLight(LightD, BufferD);
#endif

#if LIGHT_TYPE_POINT
#endif

#if LIGHT_TYPE_SPOT
#endif

#if SHADOWED
#endif

	return color;
}

#endif