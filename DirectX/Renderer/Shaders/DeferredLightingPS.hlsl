#include "HLSLGlue.h"
#include "ScreenQuadStructs.h"

//TODO 2.5d culling?
#define MAX_POINT_LIGHT 10
#define MAX_SPOT_LIGHT 10

START_CBUFFER(LightBuffer, b0)
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

START_CBUFFER(DeferredBuffer, b1) //TEMP, move to frame buffer
float4 CameraPosition;

END_CBUFFER(DeferredBuffer);

#if SHADER

SamplerState s_Sampler;
Texture2D t_Diffuse;
Texture2D t_WorldNormal;
Texture2D t_Roughness;
Texture2D t_Metallic;

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


float4 MainPS(ScreenQuadInPS input) : SV_TARGET0
{
	float4 color = t_Diffuse.Sample(s_Sampler, input.uv);
	float4 worldnormal = t_WorldNormal.Sample(s_Sampler, input.uv);
	float roughness = t_Roughness.Sample(s_Sampler, input.uv);
	float metallic = t_Metallic.Sample(s_Sampler, input.uv);

//temp dir stuff
	//float3 viewDirection = camera.xyz - 

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