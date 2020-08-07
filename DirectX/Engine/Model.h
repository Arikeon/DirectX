#pragma once
#include "Build.h"
#include "BufferKeys.h"
#include "Transform.h"
#include "Mesh.h"
#include "DebugLinesStructs.h"
#include "Renderer.h"

struct aiNode;
class CRenderer;

struct TModel
{
	void CopyFromNode(aiNode* node);

	std::vector<TMesh> m_meshes;
	std::string m_name;
	TTransform m_transform;

	//-1 if no material - default textures
	MaterialID m_materialID = -1;
};

struct TDebugLines : public TModel
{
	TDebugLines();
	int GetLineCount();
	void ClearLines();
	void AddDebugLines(float3 a, float3 b, float3 color);
	std::array<DebugLinesInVS, 4096> m_debuglines;
	int m_vertcount;

	void Initialize(CRenderer* renderer);
	bool Initialized()
	{
		return m_meshes.size() > 0;
	}
};