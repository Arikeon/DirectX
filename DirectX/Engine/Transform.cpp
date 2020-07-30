#include "Transform.h"
#include "DirectXMath.h"

TTransform::TTransform()
{
	m_position = {};
	m_rotation = {};
	m_scale = float3(1, 1, 1);
}
//chek if we can member initializer list unions
TTransform::TTransform(float2 InVector)
	: m_position(InVector.x, InVector.y, 0.f)
{
	TTransform();
}

TTransform::TTransform(float3 InVector)
	: m_position(InVector)
{

}

TTransform::TTransform(float x)
	: m_position(x, 0.f, 0.f)
{

}

TTransform::TTransform(float x, float y)
	: m_position(x, y, 0.f)
{

}

TTransform::TTransform(float x, float y, float z)
	: m_position(x, y, z)
{

}

matrix TTransform::GetMatrix(bool bUseRotation, bool bUsePosition, bool bUseScale)
{
	//Must use one
	check(bUsePosition || bUseRotation || bUseScale);

	XMMATRIX r, p, s;

	if (bUseRotation)
	{
		//Slow Path
		r = XMMatrixRotationX(m_rotation.x);
		r *= XMMatrixRotationY(m_rotation.y);
		r *= XMMatrixRotationZ(m_rotation.z);
	}
	else
		return XMMatrixIdentity();

	if (bUsePosition)
	{
		p = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	}
	else
		return XMMatrixIdentity();

	if (bUseScale)
	{
		s = XMMatrixScalingFromVector(XMLoadFloat3(&m_scale));
	}
	else
		return XMMatrixIdentity();

	return r * p * s;
}
