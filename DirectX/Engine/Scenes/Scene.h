#pragma once
#include "Build.h"
#include "Model.h"
#include "Mesh.h"
#include "Camera.h"
#include "Colors.h"
#include "Object.h"

//Passes
#include "Pass.h"

//Shaders
#include "BasePassStructs.h"

class CRenderer;

namespace EScene
{
	enum Type
	{
		eBasicShapesScene = 0,
		eCornellScene,
		eCount,
	};
}

class CScene
{
	friend class CEngine;
public:
	virtual ~CScene();
	virtual void InitializePasses(CRenderer* renderer);
	virtual void LoadScene(CRenderer* renderer) = 0;
	virtual void UpdateScene(CRenderer* renderer) = 0;
	virtual void UnloadScene() = 0;
	virtual void RenderScene(CRenderer* renderer, TWindow window, float delta);
	virtual void DrawTransform(TModel& object);
	virtual void DrawModelTransforms();
	virtual std::vector<TObject>& GetObjects() { return m_objects; }

protected:
	EScene::Type m_selectedscene;
	CCamera m_camera;
	std::vector<TObject> m_objects;
	TDebugLines m_debuglines;
	struct TBasePass* m_basepass;
	bool m_bDrawTransforms = true;
};
