#pragma once
#include "Pass.h"
#include "Camera.h"
#include "Window.h"
#include "D3D11Interface.h"
#include "BasePassStructs.h"
#include "BasePassPS.hlsl"
#include "BasePassVS.hlsl"

struct TBasePass : public TPass
{
	void Setup(CRenderer* renderer, float delta)
	{

	}

	void DrawMeshes(CRenderer* renderer,
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

		for (int i = 0; i < (int)model.m_meshes.size(); ++i)
		{
			TMesh& currMesh = model.m_meshes[i];

			if (!currMesh.m_bInitialized)
				continue;

			TD3DBuffer vbuffer;
			vbuffer = renderer->GetVertexBuffer(currMesh.m_vertexkey);

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

			if (currMesh.HasMaterial())
			{
				TMaterial& Material = renderer->GetMaterial(currMesh.m_materialKey);

				if (Material.m_textureDiffuse != -1)
				{
					TD3DTexture Diffuse = renderer->GetTexture(Material.m_textureDiffuse);

					renderer->DebugCaptureTexture(Diffuse);

					shader.SetShaderResource<EShaderStage::ePS>(context, Diffuse.m_srv);
				}

				BasePassMaterial ConstantMaterial = {};
				ConstantMaterial.DiffuseColor = float4(Material.m_diffuseColor.x, Material.m_diffuseColor.y, Material.m_diffuseColor.z, 0.f);
				ConstantMaterial.DiffuseColor = float4(1.f, 1.f, 1.f, 1.f);
				shader.WriteConstants("DiffuseColor", (void*)&ConstantMaterial.DiffuseColor);
			}

			shader.BindData(context);

			//start binding pipeline
			shader.SetShaderStages(context);
			context->OMSetRenderTargets(1, &rtv, dsv);
			context->RSSetViewports(1, &d3dview);
			context->IASetInputLayout(shader.m_inputlayout);
			context->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(currMesh.m_topology));
			UINT stride = vbuffer.m_info.m_stride;
			UINT offset = 0;

			context->IASetVertexBuffers(0, 1, &vbuffer.m_pGPUdata, &stride, &offset);

			bool bUsesIndexBuffer = currMesh.m_indexkey != -1;
			if (bUsesIndexBuffer)
			{
				TD3DBuffer ibuffer = renderer->GetIndexBuffer(currMesh.m_indexkey);
				context->IASetIndexBuffer(ibuffer.m_pGPUdata, DXGI_FORMAT_R32_UINT, 0);

				context->DrawIndexed(ibuffer.m_info.m_elementcount, 0, 0);
			}
			else
			{
				context->Draw(vbuffer.m_info.m_elementcount, 0);
			}
		}
	}

	void Render(CRenderer* renderer, std::vector<TObject>& objects, TDebugLines debuglines, CCamera camera, TWindow window, float delta)
	{
		for (unsigned int i = 0; i < (int)objects.size(); ++i)
		{
			TObject& currObject = objects[i];
			std::vector<TModel>& models = currObject.m_models;

			int nummodels = (int)models.size();
			if (nummodels == 0)
			{
				CONSOLE_LOG(L"Nothing to render..");
				return;
			}

			for (int i = 0; i < nummodels; ++i)
			{
				TModel& model = models[i];
				DrawMeshes(renderer, renderer->GetShader(EShaderList::BasePass), camera, window, delta, model);
			}

			//Draw debug lines
			if (debuglines.m_meshes.size() > 0)
			{
				TD3DBuffer vbuffer = renderer->GetVertexBuffer(debuglines.m_meshes[0].m_vertexkey);
				D3D11_MAPPED_SUBRESOURCE subresourcemap;
				ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();
				context->Map(vbuffer.m_pGPUdata, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &subresourcemap);
				memcpy(subresourcemap.pData, &debuglines.m_debuglines[0], sizeof(DebugLinesInVS) * debuglines.GetLineCount());
				context->Unmap(vbuffer.m_pGPUdata, (UINT)0);

				DrawMeshes(renderer, renderer->GetShader(EShaderList::DebugBasePass), camera, window, delta, debuglines);
			}
		}
	}
};