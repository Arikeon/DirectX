#pragma once
#include "Build.h"
#include "BufferKeys.h"
#include "Renderer.h"
#include "BufferStruct.h"
#include "D3D11Interface.h"

#include <vector>
#include <array>

namespace EVertexLayout
{
	enum Type
	{
		ePosition = 0,
		eNormal,
		eTangentsAndBiTangents,
		eUV,
		eCount,
	};
}

using TVertexLayout = std::array<bool, EVertexLayout::eCount>;

struct TMesh
{
private:
	template <class TVertexType>
	void CreateMesh(CRenderer* renderer,
		TVertexType* pvdata,
		int vertsize,
		int vertcount,
		unsigned int* pidata,
		int indisize,
		int indicount,
		EBufferUsage::Type usage = EBufferUsage::eGPU_READ_WRITE)
	{
		ID3D11Device* device = renderer->m_D3DInterface->m_device;
		check(device);

		TD3DBuffer vbuffer;

		vbuffer.m_info.m_buffertype = EBufferType::eVertexBuffer;
		vbuffer.m_info.m_bufferusage = usage;
		vbuffer.m_info.m_elementcount = vertcount;
		vbuffer.m_info.m_stride = vertsize;

		vbuffer.Initialize(device, pvdata);

		m_vertexkey = (int)renderer->m_vertexbuffers.size();
		renderer->m_vertexbuffers.push_back(vbuffer);

		if (indicount)
		{
			TD3DBuffer ibuffer;

			ibuffer.m_info.m_buffertype = EBufferType::eIndexBuffer;
			ibuffer.m_info.m_bufferusage = EBufferUsage::eGPU_READ_WRITE;
			ibuffer.m_info.m_elementcount = indicount;
			ibuffer.m_info.m_stride = indisize;


			ibuffer.Initialize(device, pidata);

			m_indexkey = (int)renderer->m_indexbuffers.size();
			renderer->m_indexbuffers.push_back(ibuffer);
		}
		if (m_vertexkey != -1)
			m_bInitialized = true;
		else
			CONSOLE_LOG(L"Failed to create mesh...");
	}

public:

	template <class TVertexType, int arraycount>
	void CreateMesh(CRenderer* renderer,
		std::array<TVertexType,
		arraycount>& verticies,
		std::vector<unsigned int>& indicies,
		EBufferUsage::Type usage = EBufferUsage::eGPU_READ_WRITE,
		ETopologyType::Type topology = ETopologyType::eTriangleList)
	{
		m_topology = topology;
		CreateMesh<TVertexType>(renderer,
			verticies.data(),
			sizeof(TVertexType),
			arraycount,
			indicies.data(),
			sizeof(unsigned int),
			static_cast<int>(indicies.size()),
			usage);
	}

	template <class TVertexType>
	void CreateMesh(CRenderer* renderer,
		std::vector<TVertexType>& verticies,
		std::vector<unsigned int>& indicies,
		EBufferUsage::Type usage = EBufferUsage::eGPU_READ_WRITE,
		ETopologyType::Type topology = ETopologyType::eTriangleList)
	{
		m_topology = topology;
		CreateMesh<TVertexType>(renderer,
			verticies.data(),
			sizeof(TVertexType),
			static_cast<int>(verticies.size()),
			indicies.data(),
			sizeof(unsigned int),
			static_cast<int>(indicies.size()),
			usage);
	}

	template <class TVertexType>
	void CreateMesh(CRenderer* renderer,
		int vertcount,
		TVertexType* verticiespdata,
		int indiciecount,
		unsigned int* intindiciespdata,
		EBufferUsage::Type usage = EBufferUsage::eGPU_READ_WRITE,
		ETopologyType::Type topology = ETopologyType::eTriangleList)
	{
		m_topology = topology;
		CreateMesh<TVertexType>(renderer,
			verticiespdata,
			sizeof(TVertexType),
			vertcount,
			intindiciespdata,
			sizeof(unsigned int),
			indiciecount,
			usage);
	}

	MaterialID m_materialKey = -1;
	BufferID m_vertexkey, m_indexkey = -1;//default if none available
	bool m_bInitialized = false;
	std::array<bool, EVertexLayout::eCount> m_vertexLayout;
	ETopologyType::Type m_topology = ETopologyType::eTriangleList;
};