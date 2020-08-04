#pragma once
#include "Pass.h"
#include "BasePassStructs.h"
#include "Camera.h"
#include "Window.h"
#include "D3D11Interface.h"

struct TBasePass : public TPass
{
	void Setup(CRenderer* renderer, float delta)
	{

	}

	void DrawModel(CRenderer* renderer,
		TShader shader,
		CCamera camera,
		TWindow window,
		float delta,
		TModel model
	)
	{
		ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();

		//main rtv and scene depth always 0
		ID3D11RenderTargetView* rtv = renderer->GetRenderTarget(0).m_rtv;
		ID3D11DepthStencilView* dsv = renderer->GetDepthTarget(0).m_dsv;

		bool bUsesIndexBuffer = model.m_mesh.m_indexkey != -1;
		TD3DBuffer vbuffer;
		vbuffer = renderer->GetVertexBuffer(model.m_mesh.m_vertexkey);

		D3D11_VIEWPORT& d3dview = renderer->GetView(EViews::eMain);
		const float aspectratio = static_cast<float>(window.m_width) / static_cast<float>(window.m_height);

		//World View Proj buffer
		BasePassWVP WVP;
		XMStoreFloat4x4(&WVP.World, model.m_transform.GetMatrix<true, true, false>());
		WVP.World._44 = 1.f;

		WVP.View = camera.m_cameramatrix;

		//Setup projection per frame
		XMStoreFloat4x4(&WVP.Proj, XMMatrixPerspectiveFovLH(window.FOV, aspectratio, 0.001f, FLT_MAX));
		shader.WriteConstants("World", (void*)&WVP.World);
		shader.WriteConstants("View", (void*)&WVP.View);
		shader.WriteConstants("Proj", (void*)&WVP.Proj);

		shader.BindData(context);

		//start binding pipeline
		shader.SetShaderStages(context);
		context->OMSetRenderTargets(1, &rtv, dsv);
		context->RSSetViewports(1, &d3dview);
		context->IASetInputLayout(shader.m_inputlayout);
		context->IASetPrimitiveTopology(model.m_topology);

		UINT stride = vbuffer.m_info.m_stride;
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, &vbuffer.m_pGPUdata, &stride, &offset);

		if (bUsesIndexBuffer)
		{
			TD3DBuffer ibuffer = renderer->GetIndexBuffer(model.m_mesh.m_indexkey);
			context->IASetIndexBuffer(ibuffer.m_pGPUdata, DXGI_FORMAT_R32_UINT, 0);

			context->DrawIndexed(ibuffer.m_info.m_elementcount, 0, 0);
		}
		else
		{
			context->Draw(vbuffer.m_info.m_elementcount, 0);
		}
	}

	void Render(CRenderer* renderer, std::vector<TModel>& models, TDebugLines debuglines, CCamera camera, TWindow window, float delta)
	{
		int nummodels = (int)models.size();
		if (nummodels == 0)
		{
			CONSOLE_LOG(L"Nothing to render..");
			return;
		}

		const auto UpdateWVP = [&](TModel model, TShader shader)
		{
		};

		for (int i = 0; i < nummodels; ++i)
		{
			TModel& model = models[i];
			if (model.IsInitialized())
				DrawModel(renderer, renderer->GetShader(EShaderList::BasePass), camera, window, delta, model);
		}

		//Draw debug lines
		{
			TD3DBuffer vbuffer = renderer->GetVertexBuffer(debuglines.m_mesh.m_vertexkey);
			D3D11_MAPPED_SUBRESOURCE subresourcemap;
			ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();
			context->Map(vbuffer.m_pGPUdata, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &subresourcemap);
			memcpy(subresourcemap.pData, &debuglines.m_debuglines[0], sizeof(DebugLinesInVS) * debuglines.GetLineCount());
			context->Unmap(vbuffer.m_pGPUdata, (UINT)0);

			DrawModel(renderer, renderer->GetShader(EShaderList::DebugBasePass), camera, window, delta, debuglines);
		}
	}
};