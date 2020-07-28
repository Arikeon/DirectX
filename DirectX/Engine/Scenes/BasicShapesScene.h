#pragma once
#include "Build.h"
#include "Scene.h"

class CBasicShapesScene : public CScene
{
	friend class CEngine;
public:
	CBasicShapesScene();
	virtual ~CBasicShapesScene();
	virtual void LoadScene(CRenderer* renderer) override final;
	virtual void UnloadScene() override final;
};