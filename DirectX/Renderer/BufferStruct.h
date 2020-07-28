#pragma once
#include <d3d11.h>

namespace BufferType
{
	enum Type
	{
		eVertexBuffer = D3D11_BIND_VERTEX_BUFFER,
		eIndexBuffer = D3D11_BIND_INDEX_BUFFER,
		eCount
	};
}

namespace BufferUsage
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

struct TBufferInfo
{
	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_buffer_desc?f1url=https%3A%2F%2Fmsdn.microsoft.com%2Fquery%2Fdev16.query%3FappId%3DDev16IDEF1%26l%3DEN-US%26k%3Dk(D3D11%2FD3D11_BUFFER_DESC)%3Bk(D3D11_BUFFER_DESC)%3Bk(DevLang-C%2B%2B)%3Bk(TargetOS-Windows)%26rd%3Dtrue
	int m_elementcount;
	int m_stride;
	int m_structurebystide;
	BufferType::Type m_buffertype;
	BufferUsage::Type m_bufferusage;
};


struct TD3DBuffer
{
	void Initialize(ID3D11Device* device, const void* pdata = nullptr)
	{
		D3D11_BUFFER_DESC bufferdesc = {};
		bufferdesc.Usage = (D3D11_USAGE)m_info.m_bufferusage;
		bufferdesc.BindFlags = (D3D11_BIND_FLAG)m_info.m_buffertype;
		bufferdesc.ByteWidth = m_info.m_stride * m_info.m_elementcount;
		bufferdesc.CPUAccessFlags = m_info.m_bufferusage;
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

	~TD3DBuffer()
	{
		delete m_pCPUdata;
		DXRelease(m_pGPUdata);
	}

	bool m_bdirty;
	void* m_pCPUdata;
	ID3D11Buffer* m_pGPUdata = nullptr;
	TBufferInfo m_info;

};
