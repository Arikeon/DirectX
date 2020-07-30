#pragma once
#include "Build.h"
#include "Model.h"
#include "Mesh.h"

//Passes
#include "Pass.h"
#include "BasePass.h"

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
	virtual void InitializePasses(CRenderer* renderer);
	virtual void LoadScene(CRenderer* renderer) = 0;
	virtual void UnloadScene() = 0;
	virtual void RenderScene(CRenderer* renderer, float delta);
	virtual std::vector<TModel>& GetModels() { return m_models; }

protected:
	EScene::Type m_selectedscene;
	std::vector<TModel> m_models;
	TBasePass m_basepass;
};
