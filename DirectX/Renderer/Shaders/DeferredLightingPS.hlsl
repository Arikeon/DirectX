#include "HLSLGlue.h"
#include "ScreenQuadStructs.h"

//TODO 2.5d culling?
#define MAX_POINT_LIGHT 10
#define MAX_SPOT_LIGHT 10

START_CBUFFER(DeferredBuffer, b0) //TEMP, move to frame buffer
float4 CameraPosition;
float4x4 InverseViewMatrix;
float4x4 InverseProjectionMatrix;
END_CBUFFER(DeferredBuffer);


START_CBUFFER(LightBuffer, b1)
float4 DirectionalColor;
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

SamplerState s_Sampler;
Texture2D t_Diffuse						: register(t0);
Texture2D t_WorldNormal					: register(t1);
Texture2D t_RoughnessMetallicDistance	: register(t2);
Texture2D t_Depth	: register(t3);

//Texture2D t_DirectionalShadow;

float4 SpecularComponentReflection(float3 ViewDirection, float SpecularPower, float3 SpecularIntensity,
	float3 Normal, float3 LightDirection, float LightIntensity)
{
	float4 SpecularComponent = float4(0.0f, 0.0f, 0.0f, 0.0f);

	if (LightIntensity > 0.0f)
	{
		float3 SpecularReflection = normalize(2 * LightIntensity * Normal + LightDirection);

		SpecularComponent = pow(saturate(dot(SpecularReflection, ViewDirection)), SpecularPower);

		//Multiply it by the intensity
		SpecularComponent = SpecularComponent * float4(SpecularIntensity, 1.0f);
	}

	return SpecularComponent;
}

float3 GetPositionVS(float2 uv, float depth)
{
	//https://mynameismjp.wordpress.com/2009/03/10/reconstructing-position-from-depth/
	float x = uv.x * 2 - 1;
	float y = (1 - uv.y) * 2 - 1;
	
	float4 projectedPosition = float4(x, y, depth, 1.0f);
	float4 viewPosition = mul(projectedPosition, InverseProjectionMatrix);

	return viewPosition.xyz / viewPosition.w;
}

float4 MainPS(ScreenQuadInPS input) : SV_TARGET0
{
	//float4 worldNormal = t_WorldNormal.Sample(s_Sampler, input.uv);
	//float roughness = t_RoughnessMetallicDistance.Sample(s_Sampler, input.uv).x;
	//float metallic = t_RoughnessMetallicDistance.Sample(s_Sampler, input.uv).y;

	float4 color = t_Diffuse.Sample(s_Sampler, input.uv);
	float distance = t_RoughnessMetallicDistance.Sample(s_Sampler, input.uv).z;

	float4 PositionVS = float4(GetPositionVS(input.uv, distance), 1.0f);

	float3 PositionWS = mul(InverseViewMatrix, PositionVS).xyz;

	//color = float4(PositionWS, 1.0f);

#if LIGHT_TYPE_DIRECTIONAL
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