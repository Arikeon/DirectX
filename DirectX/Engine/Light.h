#pragma once
#include "Transform.h"

namespace ELightType
{
	enum Type
	{
		eDirectional = 0,
		ePoint,
		eSpot,
	};
}

struct TLight
{
	TLight() {};

	float3 m_color;
	float3 m_position;
	float m_intensity;

	TTransform m_transform;
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