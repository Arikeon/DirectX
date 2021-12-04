#include "HLSLGlue.h"
#include "StructCollection.h"
#include "FrameBuffer.h"

//TODO 2.5d culling?
#define MAX_POINT_LIGHT 10
#define MAX_SPOT_LIGHT 10

START_CBUFFER(LightBuffer, b1)
float AmbientStrength;
float3 DirectionalColor;
float4 DirectionalPositionAndIntensity;
float3 DirectionalDirection;

float4 PointColor[MAX_POINT_LIGHT];
float4 PointPositionAndIntensity[MAX_POINT_LIGHT];

float4 SpotColor[MAX_SPOT_LIGHT];
float4 SpotPositionAndIntensity[MAX_SPOT_LIGHT];
float4 SpotDirection[MAX_SPOT_LIGHT];

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


float4 MainPS(ScreenQuadInPS input) : SV_TARGET0
{
	float4 worldNormal = t_WorldNormal.Sample(s_Sampler, input.uv);
	float roughness = t_RoughnessMetallicSpecular.Sample(s_Sampler, input.uv).x;
	float metallic = t_RoughnessMetallicSpecular.Sample(s_Sampler, input.uv).y;

	const float SpecularIntensity = 1.0f;
	const int SpecularExponent = 64;


	float4 color = t_Diffuse.Sample(s_Sampler, input.uv);
	float distance = t_Depth.Sample(s_Sampler, input.uv).r;

	float3 worldPos = GetPositionVS(input.uv, distance);
	worldPos = mul(InverseView, worldPos) + CameraPosition;

#if LIGHT_TYPE_DIRECTIONAL
		PhongShade(color,
			DirectionalPositionAndIntensity,
			DirectionalColor,
			DirectionalPositionAndIntensity.w,
			worldPos,
			worldNormal.xyz,
			roughness,
			metallic,
			SpecularIntensity,
			SpecularExponent);
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