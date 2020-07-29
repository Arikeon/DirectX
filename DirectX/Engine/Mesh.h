#pragma once
#include "Build.h"
#include "BufferKeys.h"
#include "Renderer.h"
#include "BufferStruct.h"
#include "D3D11Interface.h"
#include <vector>

struct TMesh
{
	template <class TVertexType>
	void CreateMesh(CRenderer* renderer, std::vector<TVertexType>& verticies, std::vector<unsigned int>& indicies)
	{
		ID3D11Device* device = renderer->m_D3DInterface->m_device;
		check(device);

		//fix this memopry leak!
		TD3DBuffer vbuffer;

		vbuffer.m_info.m_buffertype = EBufferType::eVertexBuffer;
		vbuffer.m_info.m_bufferusage = EBufferUsage::eGPU_READ_WRITE;
		vbuffer.m_info.m_elementcount = (int)verticies.size();
		vbuffer.m_info.m_stride = sizeof(verticies[0]);


		vbuffer.Initialize(device, verticies.data());

		m_vertexkey = (int)renderer->m_vertexbuffers.size();
		renderer->m_vertexbuffers.push_back(vbuffer);

		if (indicies.size())
		{
			TD3DBuffer ibuffer;

			ibuffer.m_info.m_buffertype = EBufferType::eIndexBuffer;
			ibuffer.m_info.m_bufferusage = EBufferUsage::eGPU_READ_WRITE;
			ibuffer.m_info.m_elementcount = (int)indicies.size();
			ibuffer.m_info.m_stride = sizeof(unsigned int);


			ibuffer.Initialize(device, indicies.data());

			m_indexkey = (int)renderer->m_indexbuffers.size();
			renderer->m_indexbuffers.push_back(ibuffer);
		}
	}

	BufferID m_vertexkey, m_indexkey = -1;//default if none available
	bool m_bInitialized = false;
};