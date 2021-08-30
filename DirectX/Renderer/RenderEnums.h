#pragma once
#include <d3d11.h>

//PassInfos
namespace ERasterizerStates
{
	enum Type
	{
		eFillSolidCullBack = 0,
		eFillSolidCullFront,
		eFillSolidCullNone,
		eFillWireframeCullNone,
		eCount
	};
}

namespace EViews
{
	enum Type
	{
		eMain = 0,

		eCount
	};
}

//Buffers
namespace EBufferType
{
	enum Type
	{
		eVertexBuffer = D3D11_BIND_VERTEX_BUFFER,
		eIndexBuffer = D3D11_BIND_INDEX_BUFFER,
		eCount
	};
}

namespace EBufferUsage
{
	enum Type
	{
		eGPU_READ = D3D11_USAGE_IMMUTABLE,
		eGPU_READ_WRITE = D3D11_USAGE_DEFAULT,
		eGPU_READ_CPU_WRITE = D3D11_USAGE_DYNAMIC,
		eGPU_READ_CPU_READ = D3D11_USAGE_STAGING,
		eCount
	};
}

namespace ETopologyType
{
	enum Type
	{
		eLineList = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		eTriangleList = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		eCount
	};
}

namespace EFormat
{
	enum Type
	{
		eRGBA32F = DXGI_FORMAT_R32G32B32A32_FLOAT,
		eRGBA16F = DXGI_FORMAT_R16G16B16A16_FLOAT,
		eRGBA8UN = DXGI_FORMAT_R8G8B8A8_UNORM,
		eRGB32F = DXGI_FORMAT_R32G32B32_FLOAT,
		eR11G11B10F = DXGI_FORMAT_R11G11B10_FLOAT,
		eRG32F = DXGI_FORMAT_R32G32_FLOAT,
		eRG16F = DXGI_FORMAT_R16G16_FLOAT,
		eR32F = DXGI_FORMAT_R32_FLOAT,
		eR32U = DXGI_FORMAT_R32_UINT,
		eR8U = DXGI_FORMAT_R8_UINT,
		eR8UN = DXGI_FORMAT_R8_UNORM,
		eR32 = DXGI_FORMAT_R32_TYPELESS,
		eR24G8 = DXGI_FORMAT_R24G8_TYPELESS,
		eR24_UNORM_X8_TYPELESS = DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
		eD32F = DXGI_FORMAT_D32_FLOAT,
		eD24UNORM_S8U = DXGI_FORMAT_D24_UNORM_S8_UINT,
		eNULL,
		eCount
	};
}

namespace EGBufferKeys
{
	enum Key
	{
		eBaseColor = 0,
		eWorldNormal,
		eRoughness,
		eMetallic,
		eMax
	};
}
