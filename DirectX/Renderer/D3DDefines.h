#if __cplusplus
#pragma once
#include <typeinfo>
#include <fstream>
#include <d3d11.h>
#include <DirectXMath.h>

#pragma comment(lib,"d3d11.lib")

using namespace DirectX;

typedef XMUINT2		uint2;
typedef XMUINT3		uint3;
typedef XMUINT4		uint4;
typedef XMINT2		int2;
typedef XMINT3		int3;
typedef XMINT4		int4;
typedef XMFLOAT2	float2;
typedef XMFLOAT3	float3;
typedef XMFLOAT4	float4;
typedef XMFLOAT4X4	float4x4;
typedef XMFLOAT3X3	float3x3;
typedef XMMATRIX	matrix;
#endif