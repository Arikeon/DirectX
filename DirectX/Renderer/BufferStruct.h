#pragma once
#include "RenderEnums.h"
#include "Build.h"
#include "D3DDefines.h"

#include <d3d11.h>

struct TD3DBuffer;
struct TBufferInfo;
struct TD3DTexture;
struct TD3DSampler;
struct TD3DRenderTarget;
struct TMaterial;

struct TMaterial
{
	void Release()
	{

	}

	float3 m_diffuseColor;
	float3 m_specularColor;
	float m_opacity;
	float m_reflectivity;

	static int GetConstantSize()
	{
		return
			sizeof(m_diffuseColor) +
			sizeof(m_specularColor) +
			sizeof(m_reflectivity) +
			sizeof(m_opacity);
	}

	TextureID m_textureDiffuse = -1;
	TextureID m_textureNormal = -1;
	TextureID m_textureMask = -1;
	TextureID m_textureSpecular = -1;
};

struct TD3DSampler
{
	void Release()
	{
		DXRelease(m_samplerstate);
	}

	ID3D11SamplerState* m_samplerstate;
	std::string m_name;
	SamplerID m_id;
};

struct TD3DTexture
{
	void Release()
	{
		DXRelease(m_srv);
		DXRelease(m_uav);

		DXArrayRelease(m_srvarr);
		DXArrayRelease(m_uavarr);

		DXRelease(m_tex2D);
	}

	int m_with, m_height, m_depth;
	TextureID m_id;
	std::string m_name;

	ID3D11ShaderResourceView* m_srv = nullptr;
	ID3D11UnorderedAccessView* m_uav = nullptr;

	//cube
	std::vector<ID3D11ShaderResourceView*> m_srvarr;
	std::vector<ID3D11UnorderedAccessView*> m_uavarr;

	union
	{
		ID3D11Texture2D* m_tex2D;
		ID3D11Texture3D* m_tex3D;
	};
};

struct TD3DDepthTarget
{
	void Release()
	{
		DXRelease(m_dsv);
		D3DRelease(m_texture);
	}

	TD3DTexture m_texture;
	ID3D11DepthStencilView* m_dsv = nullptr;
};

struct TD3DRenderTarget
{
	void Release()
	{
		DXRelease(m_rtv);
		D3DRelease(m_texture);
	}

	TD3DTexture m_texture;
	ID3D11RenderTargetView* m_rtv = nullptr;
};

struct TBufferInfo
{
	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_buffer_desc?f1url=https%3A%2F%2Fmsdn.microsoft.com%2Fquery%2Fdev16.query%3FappId%3DDev16IDEF1%26l%3DEN-US%26k%3Dk(D3D11%2FD3D11_BUFFER_DESC)%3Bk(D3D11_BUFFER_DESC)%3Bk(DevLang-C%2B%2B)%3Bk(TargetOS-Windows)%26rd%3Dtrue
	int m_elementcount;
	int m_stride;
	int m_structurebystide;
	EBufferType::Type m_buffertype;
	EBufferUsage::Type m_bufferusage;
};

struct TD3DBuffer
{
	void Initialize(ID3D11Device* device, const void* pdata = nullptr)
	{
		D3D11_BUFFER_DESC bufferdesc = {};
		bufferdesc.Usage = (D3D11_USAGE)m_info.m_bufferusage;
		bufferdesc.BindFlags = (D3D11_BIND_FLAG)m_info.m_buffertype;
		bufferdesc.ByteWidth = m_info.m_stride * m_info.m_elementcount;
		bufferdesc.CPUAccessFlags = (m_info.m_bufferusage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
		bufferdesc.MiscFlags = 0;
		bufferdesc.StructureByteStride = m_info.m_structurebystide;

		D3D11_SUBRESOURCE_DATA* psubdata = nullptr;
		if (pdata)
		{
			D3D11_SUBRESOURCE_DATA subdata = {};
			subdata.pSysMem = pdata;
			psubdata = &subdata;

			m_pCPUdata = malloc(bufferdesc.ByteWidth);
			memcpy(m_pCPUdata, pdata, bufferdesc.ByteWidth);
		}

		HRESULT r;
		r = device->CreateBuffer(&bufferdesc, psubdata, &m_pGPUdata);
		checkhr(r);
	}

	void Release()
	{
		if (m_pCPUdata)
		{
			delete m_pCPUdata;
			m_pCPUdata = nullptr;
		}
		DXRelease(m_pGPUdata);
	}

	bool m_bdirty;
	void* m_pCPUdata = nullptr;
	ID3D11Buffer* m_pGPUdata = nullptr;
	TBufferInfo m_info;

};
