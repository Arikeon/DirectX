#pragma once
#include "Pass.h"
#include "BasePassStructs.h"
#include "Camera.h"
#include "Window.h"

struct TBasePass : public TPass
{
	void Setup(CRenderer* renderer, float delta)
	{

	}

	void Render(CRenderer* renderer,std::vector<TModel>& models, CCamera camera, TWindow window, float delta)
	{
		TShader BasePassShader = renderer->GetShader(ShaderID::BasePass);

		int nummodels = (int)models.size();
		if (nummodels == 0)
		{
			CONSOLE_LOG(L"Nothing to render..");
			return;
		}

		for (int i = 0; i < nummodels; ++i)
		{
			TModel& model = models[i];

						BasePassWVP WVP;
			XMStoreFloat4x4(&WVP.World, model.m_transform.GetMatrix());
			WVP.View = camera.m_cameramatrix;

			//Setup projection per frame
			const int aspectratio = window.m_width / window.m_height;
			D3D11_VIEWPORT& d3dview = renderer->GetView(EViews::eMain);

			XMStoreFloat4x4(&WVP.Proj, XMMatrixPerspectiveFovLH(window.FOV, (float)aspectratio, d3dview.MinDepth, d3dview.MaxDepth));
		}
	}
};