#pragma once
#include "Build.h"
#include "D3DDefines.h"

struct TTransform
{
	TTransform();
	TTransform(float2 InVector);
	TTransform(float3 InVector);
	TTransform(float x);
	TTransform(float x, float y);
	TTransform(float x, float y, float z);

	matrix GetMatrix(bool bUseRotation = false, bool bUsePosition = false, bool bUseScale = false);

	float3 m_position;
	float3 m_rotation;
	float3 m_scale;
};
