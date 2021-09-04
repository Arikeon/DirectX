#pragma once
#include "Build.h"
#include "BufferKeys.h"
#include "Transform.h"
#include "Mesh.h"
#include "StructCollection.h"
#include "Renderer.h"
#include "RenderEnums.h"

struct aiNode;
class CRenderer;

struct TModel
{
	void CopyFromNode(aiNode* node);

	std::vector<TMesh> m_meshes;
	std::string m_name;
	TTransform m_transform;
	ERasterizerStates::Type m_rasterizationState = ERasterizerStates::eFillSolidCullBack;
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