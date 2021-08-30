#pragma once
#include "Transform.h"

struct TLight
{
	TLight() {};

	float3 m_color;
	float3 m_position;
	float m_intensity;
	bool m_shadowed;

	TTransform m_transform;
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