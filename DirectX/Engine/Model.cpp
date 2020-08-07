#include "Model.h"

//min max compile errors - undef them
//{
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#undef max
#undef min
//}

//assimp library
#include <assimp\scene.h>

void TModel::CopyFromNode(aiNode* node)
{
	m_name = node->mName.C_Str();
	float4x4 transformMatrix;
	memcpy(&transformMatrix, &node->mTransformation, sizeof(float4x4));
	m_transform = TTransform(transformMatrix);

	//Reset rotation
	m_transform.m_rotation = {};
}


int TDebugLines::GetLineCount()
{
	return m_vertcount;
}

void TDebugLines::ClearLines()
{
	m_vertcount = 0;
}

void TDebugLines::AddDebugLines(float3 a, float3 b, float3 color)
{
	if (m_meshes.size() > 0)
	{
		m_debuglines[m_vertcount].position = a;
		m_debuglines[m_vertcount++].color = color;
		m_debuglines[m_vertcount].position = b;
		m_debuglines[m_vertcount++].color = color;
	}
}

TDebugLines::TDebugLines()
{
	m_name = "Debug lines";
	m_transform.m_position = { 0, 0, 0 };
	m_debuglines = {};
}

void TDebugLines::Initialize(CRenderer* renderer)
{
	m_meshes.resize(1);
	std::vector<unsigned int> emptyindexbuffer;
	TMesh& mesh = m_meshes[0];
	mesh.CreateMesh<DebugLinesInVS>(renderer, m_debuglines, emptyindexbuffer, EBufferUsage::eGPU_READ_CPU_WRITE, ETopologyType::eLineList);
}