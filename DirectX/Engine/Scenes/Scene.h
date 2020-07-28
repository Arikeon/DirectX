#pragma once
#include "Build.h"
#include "Model.h"
#include "Mesh.h"

class CRenderer;

namespace EScene
{
	enum Type
	{
		eBasicShapesScene = 0,

		eCount,
	};
}

class CScene
{
	friend class CEngine;
public:
	virtual ~CScene() {};
	virtual void LoadScene(CRenderer* renderer) = 0;
	virtual void UnloadScene() = 0;

protected:
	EScene::Type m_selectedscene;
	std::vector<TModel> m_models;
};
