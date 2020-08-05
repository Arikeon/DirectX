#include "Transform.h"
#include "DirectXMath.h"

TTransform::TTransform(float4x4 inTransform)
{
	//This assumes there is no scale
	m_position = float3(inTransform._41, inTransform._42, inTransform._43);
	m_rotation = float3(inTransform._11, inTransform._22, inTransform._33);
}

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