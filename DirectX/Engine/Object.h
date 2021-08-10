#pragma once
#include "Model.h"
#include "Transform.h"

struct aiNode;

struct TObject
{
	TObject() {};
	void CopyFromNode(aiNode* node);
	void TransformMeshes(float3 offset);
	void RotateMeshes(float3 offset);
	void ScaleMeshes(float3 scale);

	std::string GetName() { return m_name; }

	std::string m_name;
	std::vector<TModel> m_models;
};