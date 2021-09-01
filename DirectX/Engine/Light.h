#pragma once
#include "Transform.h"

namespace ELightType
{
	enum Type
	{
		eDirectional = 0,
		ePoint,
		eSpot
	};
}

struct TLight
{
	TLight() {};

	float3 m_color;
	float m_intensity;
	bool m_shadowed = false;

	float3 m_position = {};
	float3 m_direction = {};

	ELightType::Type m_lightType;
};

struct TDirectionalLight : public TLight
{
	//TODO
};

struct TPointLight : public TLight
{

};

struct TSpotLight : public TLight
{
	//TODO
};