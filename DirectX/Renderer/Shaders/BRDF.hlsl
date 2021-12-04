#include "HLSLGlue.h"

#if 1//SHADER

	//https://learnopengl.com/PBR/Theory
float DistributionGGX(float3 N, float3 HDir, float roughness)
{
	float a2 = roughness * roughness;
	float NoH = max(dot(N, HDir), 0.0f);
	NoH = NoH * NoH;
	
	float denom = (NoH * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;
	
	return a2 / denom;
}

	//https://learnopengl.com/PBR/Theory
float GeometrySchlickGGX(float NoV, float roughness)
{
	float nom = NoV;
	float denom = NoV * (1.0 - roughness) + roughness;
	
	return nom / denom;
}

	//https://learnopengl.com/PBR/Theory
float GeometrySmith(float3 N, float3 VDir, float NoV, float NoL, float LDir, float roughness)
{
	float ggx1 = GeometrySchlickGGX(NoV, roughness);
	float ggx2 = GeometrySchlickGGX(NoL, roughness);
	
	return ggx1 * ggx2;
}

	//https://learnopengl.com/PBR/Theory
float3 FresnelSchlick(float NoLH, float3 color, float metallic)
{
	float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), color, 0.0f);
	return F0 + (1.0f - F0) * pow(1.0f - NoLH, 5.0f);
}

void PhongShade(inout float4 pixelColor,
	float3 LPosition,
	float3 LColor,
	float LIntensity,
	float3 pixelPosition,
	float3 N,
	float roughness,
	float metallic,
	float specularPower,
	int specularExponent)
{
	float3 LDir = normalize(LPosition - pixelPosition);
	float3 VDir = normalize(CameraPosition.xyz - pixelPosition);
	float3 HDir = normalize(LDir + VDir);
	
	float NoL = saturate(dot(N, LDir));
	float NoLH = saturate(dot(N, HDir));
	float NoV = saturate(dot(N, VDir));
	float HoV = saturate(dot(HDir, VDir));
	
	float distance = length(LPosition - pixelPosition);
	float attenuation = 1.0f / (distance * distance);
	float radiance = pixelColor * attenuation;
	
	
	//NoLH = NoL != 0.0f ? NoLH : 0.0f;
	//NoLH = pow(NoLH, specularExponent);
	
	float NDF = DistributionGGX(N, HDir, roughness);
	float G = GeometrySmith(N, VDir, NoV, NoL, LDir, roughness);
	float3 F = FresnelSchlick(HoV, pixelColor, metallic);
	
	float numerator = NDF * G * F;
	float denominator = 4.f * max(NoV * NoL, 0.0f) + EPSILON;
	
	float3 specular = numerator / denominator;
	
	float3 kS = F;
	float3 kD = float3(1.f, 1.f, 1.f) - kS;
	kD *= 1.0f - metallic;

	pixelColor.xyz = (kD * pixelColor / PI + specular) * radiance * NoL;
}

#endif