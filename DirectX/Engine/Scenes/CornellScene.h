#pragma once
#include "Build.h"
#include "Scene.h"

class CCornellScene : public CScene
{
	friend class CEngine;

public:
	CCornellScene();
	virtual ~CCornellScene();
	virtual void UpdateScene(CRenderer* renderer, float delta) override final;
	virtual void LoadScene(CRenderer* renderer) override final;
	virtual void UnloadScene() override final;
};