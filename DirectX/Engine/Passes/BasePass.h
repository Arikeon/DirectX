#pragma once
#include "Pass.h"
#include "BasePassStructs.h"

struct TBasePass : public TPass
{
	void Setup(CRenderer* renderer, float delta)
	{

	}

	void Render(CRenderer* renderer, std::vector<TModel>& models, float delta)
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

		}
	}
};