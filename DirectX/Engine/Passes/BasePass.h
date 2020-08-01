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

	void Render(CRenderer* renderer,std::vector<TModel>& models, CCamera camera, TWindow window, float delta)
	{
		int nummodels = (int)models.size();
		if (nummodels == 0)
		{
			CONSOLE_LOG(L"Nothing to render..");
			return;
		}

		TShader BasePassShader = renderer->GetShader(ShaderID::BasePass);
		ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();

		//main rtv and scene depth always 0
		ID3D11RenderTargetView* rtv = renderer->GetRenderTarget(0).m_rtv;
		ID3D11DepthStencilView* dsv = renderer->GetDepthTarget(0).m_dsv;

		for (int i = 0; i < nummodels; ++i)
		{
			TModel& model = models[i];
			TMesh& mesh = model.m_mesh;

			bool bUsesIndexBuffer = mesh.m_indexkey != -1;
			TD3DBuffer vbuffer;
			vbuffer = renderer->GetVertexBuffer(mesh.m_vertexkey);

			const float aspectratio = static_cast<float>(window.m_width) / static_cast<float>(window.m_height);
			D3D11_VIEWPORT& d3dview = renderer->GetView(EViews::eMain);

			//World View Proj buffer
			{
				BasePassWVP WVP;
				XMStoreFloat4x4(&WVP.World, model.m_transform.GetMatrix<true, true, false>());
				WVP.World._44 = 1.f;

				WVP.View = camera.m_cameramatrix;

				//Setup projection per frame
				XMStoreFloat4x4(&WVP.Proj, XMMatrixPerspectiveFovLH(window.FOV, aspectratio, 0.001f, FLT_MAX));
				BasePassShader.WriteConstants("World", (void*)&WVP.World);
				BasePassShader.WriteConstants("View", (void*)&WVP.View);
				BasePassShader.WriteConstants("Proj", (void*)&WVP.Proj);
			}
			BasePassShader.BindData(context);

			//start binding pipeline
			BasePassShader.SetShaderStages(context);
			context->OMSetRenderTargets(1, &rtv, dsv);
			context->RSSetViewports(1, &d3dview);
			context->IASetInputLayout(BasePassShader.m_inputlayout);
			context->IASetPrimitiveTopology(mesh.m_topology);

			UINT stride = vbuffer.m_info.m_stride;
			UINT offset = 0;

			context->IASetVertexBuffers(0, 1, &vbuffer.m_pGPUdata, &stride, &offset);

			if (bUsesIndexBuffer)
			{
				TD3DBuffer ibuffer = renderer->GetIndexBuffer(mesh.m_indexkey);
				context->IASetIndexBuffer(ibuffer.m_pGPUdata, DXGI_FORMAT_R32_UINT, 0);

				context->DrawIndexed(ibuffer.m_info.m_elementcount, 0, 0);
			}
			else
			{
				context->Draw(vbuffer.m_info.m_elementcount, 0);
			}
		}
	}
};