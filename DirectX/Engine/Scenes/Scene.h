#pragma once
#include "Build.h"
#include "Model.h"
#include "Mesh.h"
#include "Camera.h"
#include "Colors.h"
#include "Object.h"
#include "Light.h"

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
	virtual void Initialize(CRenderer* renderer);
	virtual void LoadScene(CRenderer* renderer) = 0;
	virtual void UpdateScene(CRenderer* renderer) = 0;
	virtual void UnloadScene() = 0;
	virtual void RenderScene(CRenderer* renderer, TWindow window, float delta);
	virtual void DrawTransform(TModel& object);
	virtual void DrawModelTransforms();
	virtual std::vector<TObject>& GetObjects() { return m_objects; }

protected:
	virtual void InitializeScreenQuad(CRenderer* renderer);

	EScene::Type m_selectedscene;
	CCamera m_camera;
	std::vector<TObject> m_objects;
	std::vector<TDirectionalLight> m_DirectionalLights;
	std::vector<TPointLight> m_PointLights;
	std::vector<TSpotLight> m_SpotLights;
	TObject m_ScreenQuad;
	TDebugLines m_debuglines;
	bool m_bDrawTransforms = true;
};
