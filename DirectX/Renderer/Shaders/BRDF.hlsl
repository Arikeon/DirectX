#include "HLSLGlue.h"

#if 1//SHADER

#define GAMMA_CORRECTION 1
#define BLINN_PHONG 1
#define PHONG !BLINN_PHONG

	//https://learnopengl.com/PBR/Theory
float DistributionGGX(float NoH, float roughness)
{
	float a2 = roughness * roughness;
	a2 = a2 * a2;
	
	float NoH2 = NoH;
	NoH2 = NoH2 * NoH2;
	
	float denom = (NoH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;
	
	return a2 / denom;
}

	//https://learnopengl.com/PBR/Theory
float GeometrySchlickGGX(float NoV, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r * r) / 8.0f;
	
	float nom = NoV;
	float denom = (NoV * (1.0 - k) + k) + EPSILON;
	
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
float3 FresnelSchlick(float NoLH, float3 albedo, float metallic)
{
	float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);
	return F0 + (1.0f - F0) * pow(clamp(1.0f - NoLH, 0.0f, 1.0f), 5.0f);
}

float3 PhongShade(inout float4 pixelColor,
	float3 LPosition,
	float3 LColor,
	float LIntensity,
	float3 pixelPosition,
	float3 N,
	float roughness,
	float metallic,
	float specularPower,
	bool directional) //TODO remove this hack
{
	float3 albedo = pixelColor;
	float3 ambient = albedo * AmbientStrength;
	float3 LDir = normalize(LPosition - pixelPosition);
	float3 VDir = normalize(CameraPosition.xyz - pixelPosition);
	float3 HDir = normalize(LDir + VDir);
	
	float NoL = saturate(dot(N, LDir));
	float NoH = saturate(dot(N, HDir));
	float NoV = saturate(dot(N, VDir));
	float HoV = saturate(dot(HDir, VDir));
	
	float distance = length(LPosition - pixelPosition);
	float attenuation = 1.0f / (distance * distance);
	float radiance = albedo * LIntensity;
	if (!directional)
		radiance = radiance * attenuation;
	
	float NDF = DistributionGGX(NoH, roughness);
	float G = GeometrySmith(N, VDir, NoV, NoL, LDir, roughness);
	float3 F = FresnelSchlick(HoV, albedo, metallic);
	
	float denominator = 4.f * max(NoV * NoL, 0.0f) + 0.0001f;
	float3 specular = (NDF * G * F) / denominator * specularPower;
	
	float3 kS = F;
	float3 kD = (float3(1.f, 1.f, 1.f) - kS) * (1.0f - metallic) * albedo;

	float3 output = kD / PI + specular + ambient;
	
#if BLINN_PHONG
	output = output * radiance * NoH;
#elif PHONG
	output = output * radiance * NoL;
#endif
	
	output = output / (output + float3(1.0f, 1.0f, 1.0f));
	
#if GAMMA_CORRECTION
	float Ga = 1.0f / 2.2f;
	float3 Gamma = float3(Ga, Ga, Ga);
	
	output = pow(output, Gamma);
#endif
	
	return output;
}

#endif