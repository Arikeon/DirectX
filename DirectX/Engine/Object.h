#pragma once
#include "Model.h"
#include "Transform.h"

struct aiNode;

struct TObject
{
	TObject() {};
	void CopyFromNode(aiNode* node);
	std::string GetName() { return m_name; }

	std::string m_name;
	std::vector<TModel> m_models;
	TTransform m_transform;
};