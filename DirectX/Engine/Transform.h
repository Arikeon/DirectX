#pragma once
#include "Build.h"
#include "D3DDefines.h"

struct TTransform
{
	TTransform();
	TTransform(float4x4 InTransform);
	TTransform(float2 InVector);
	TTransform(float3 InVector);
	TTransform(float x);
	TTransform(float x, float y);
	TTransform(float x, float y, float z);

	template<bool bUseRotation = false, bool bUsePosition = false, bool bUseScale = false>
	matrix GetMatrix()
	{
		//Must use one
		check(bUsePosition || bUseRotation || bUseScale);

		matrix r, p, s;
		r = p = s = XMMatrixIdentity();


		if (bUseRotation)
		{
			//Slow Path
			r = XMMatrixRotationX(m_rotation.x);
			r *= XMMatrixRotationY(m_rotation.y);
			r *= XMMatrixRotationZ(m_rotation.z);
		}

		if (bUsePosition)
		{
			p = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
		}

		if (bUseScale)
		{
			s = XMMatrixScalingFromVector(XMLoadFloat3(&m_scale));
		}

		return r * p * s;
	}

	float3 m_position = {};
	float3 m_rotation = {};
	float3 m_scale = {};
};
