#pragma once
#include "Build.h"
#include "BufferKeys.h"
#include "Transform.h"
#include "Mesh.h"
#include "DebugLinesStructs.h"

struct TModel
{
	TMesh m_mesh;
	std::string m_name;
	TTransform m_transform;
	D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

struct TDebugLines : public TModel
{
	int GetLineCount()
	{
		return m_vertcount;
	}

	void ClearLines()
	{
		m_vertcount = 0;
	}

	void AddDebugLines(float3 a, float3 b, float3 color)
	{
		m_debuglines[m_vertcount].position = a;
		m_debuglines[m_vertcount++].color= color;
		m_debuglines[m_vertcount].position = b;
		m_debuglines[m_vertcount++].color= color;
	}

	TDebugLines()
	{
		m_name = "Debug lines";
		m_transform.m_position = { 0, 0, 0 };
		m_topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		m_debuglines = {};
	};

	std::array<DebugLinesInVS, 4096> m_debuglines;
	int m_vertcount;
};