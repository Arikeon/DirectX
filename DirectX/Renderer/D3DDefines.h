#if __cplusplus
#pragma once
#include <typeinfo>
#include <fstream>
#include <d3d11.h>
#include <DirectXMath.h>

#pragma comment(lib,"d3d11.lib")

using namespace DirectX;

#if 1 //this is a headach
#define _D3DEngineStruct3(Name, Type) struct _##Name##3 : public Name##3 { \
_##Name##3() = default; \
_##Name##3(_##Name##3&& xyz) = default; \
_##Name##3(const _##Name##3& xyz) = default; \
_##Name##3& operator=(const _##Name##3&) = default; \
_##Name##3& operator=(_##Name##3&&) = default; \
explicit _##Name##3(##Name##2 xy, Type z)	: Name##3(xy.x, xy.y, z) { } \
explicit _##Name##3(Type x, ##Name##2 yz)	: Name##3(x, yz.x, yz.y) { } \
explicit _##Name##3(##Name##2 xy)			: Name##3(xy.x, xy.y, (Type)0) { } \
explicit _##Name##3(Type x, Type y, Type z) : Name##3(x, y, z) { }};

#define _D3DEngineStruct4(Name, Type) struct _##Name##4 : public Name##4 { \
_##Name##4() = default; \
_##Name##4(_##Name##4&& xyz) = default; \
_##Name##4(const _##Name##4& xyz) = default; \
_##Name##4& operator=(const _##Name##4&) = default; \
_##Name##4& operator=(_##Name##4&&) = default; \
explicit _##Name##4(##Name##3 xyz, Type w)			: Name##4(xyz.x, xyz.y, xyz.z, w) { } \
explicit _##Name##4(Type x, ##Name##3 yzw)			: Name##4(x, yzw.x, yzw.y, yzw.z) { } \
explicit _##Name##4(##Name##2 xy, Type z, Type w)	: Name##4(xy.x, xy.y, z, w) { } \
explicit _##Name##4(Type x, Type y, ##Name##2 zw)	: Name##4(x, y, zw.x ,zw.y) { } \
explicit _##Name##4(##Name##2 xy, ##Name##2 zw)		: Name##4(xy.x, xy.y, zw.x, zw.y) { } \
explicit _##Name##4(##Name##2 xy)					: Name##4(xy.x, xy.y, (Type)0, (Type)0) { } \
explicit _##Name##4(##Name##3 xyz)					: Name##4(xyz.x, xyz.y, xyz.z, (Type)0) { } \
explicit _##Name##4(Type x, Type y, Type z, Type w)	: Name##4(x, y, z, w) { }};

_D3DEngineStruct3(XMUINT, unsigned int)
_D3DEngineStruct3(XMINT, int)
_D3DEngineStruct3(XMFLOAT, float)
_D3DEngineStruct4(XMUINT, unsigned int)
_D3DEngineStruct4(XMINT, int)
_D3DEngineStruct4(XMFLOAT, float)

typedef XMUINT2		uint2;
typedef _XMUINT3	uint3;
typedef _XMUINT4	uint4;
typedef XMINT2		int2;
typedef _XMINT3		int3;
typedef _XMINT4		int4;
typedef XMFLOAT2	float2;
typedef _XMFLOAT3	float3;
typedef _XMFLOAT4	float4;
typedef XMFLOAT4X4	float4x4;
typedef XMFLOAT3X3	float3x3;
typedef XMMATRIX	matrix;
typedef XMVECTOR	vector;

#else

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
typedef XMVECTOR	vector;
#endif
#endif