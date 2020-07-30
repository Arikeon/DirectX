#pragma once
#include "Build.h"
#include "BufferKeys.h"
#include "Transform.h"
#include "Mesh.h"

struct TModel
{
	TModel()
	{
	};
	TMesh m_mesh;
	std::string m_name;
	TTransform m_transform;
};