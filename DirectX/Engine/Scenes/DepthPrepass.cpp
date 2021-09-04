#include "DepthPrepass.h"
#include "StructCollection.h"
#include "DepthVS.hlsl"
#include "Shader.h"
#include "FrameBuffer.h"

void TDepthPrePass::Render(CRenderer* renderer,
	std::vector<TObject>& objects,
	TDebugLines debuglines,
	CCamera camera,
	TWindow window,
	float delta)
{
	ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();
	TShader& DepthPrePassShader = renderer->GetShader(EShaderList::eDepth);
	renderer->UnbindRTV();

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
			DrawMeshes(renderer, DepthPrePassShader, camera, window, delta, model);
		}
	}

	renderer->UnbindSRV(DepthPrePassShader);
}


void TDepthPrePass::DrawMeshes(CRenderer* renderer,
	TShader shader,
	CCamera camera,
	TWindow window,
	float delta,
	TModel model)
{
	ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();
	ID3D11DepthStencilView* dsv = renderer->GetGBufferDepth().m_dsv;
	ID3D11DepthStencilState* dss = renderer->GetDepthStencilState((int32)EDepthStencilStates::ePrePass).m_dss;
	D3D11_VIEWPORT& d3dview = renderer->GetView(EViews::eMain);
	FrameBuffer& frameBuffer = renderer->GetFrameBuffer();

	const int32 numRTVs = 1;
	ID3D11RenderTargetView* rtvs[numRTVs];

	check(numRTVs <= RTVSLOTMAX);
	rtvs[0] = renderer->GetGBufferRTV(EGBufferKeys::eRoughnessMetallicSpecular).m_rtv;

	for (int i = 0; i < (int)model.m_meshes.size(); ++i)
	{
		TMesh& currMesh = model.m_meshes[i];

		if (!currMesh.m_bInitialized || currMesh.m_vertexdepthkey == -1)
			continue;

		TD3DBuffer vbuffer;
		vbuffer = renderer->GetVertexBuffer(currMesh.m_vertexdepthkey);

		bool bUsesInstanceBuffer = currMesh.HasInstances();

		int32 NumInstances = 0;
		int32 permutationIndex = 0;
		std::vector<PermutationValue> permutationKey;

		permutationKey.push_back({ "USE_INSTANCING", (int32)bUsesInstanceBuffer });
		permutationIndex = shader.m_permutations.GetShaderWithPermutation(permutationKey);


		//________________Map Vertex & Instance data
		{
			//World View Proj buffer

			matrix world, view, projection;

			world = model.m_transform.GetMatrix<true, false, true>();
			view = XMLoadFloat4x4(&frameBuffer.View);
			projection = XMLoadFloat4x4(&frameBuffer.Projection);

			float4x4 WorldViewProjection;
			XMStoreFloat4x4(&WorldViewProjection, (world * view) * projection);

			shader.WriteConstants("WorldViewProjection", (void*)&WorldViewProjection, permutationIndex);

			if (bUsesInstanceBuffer)
			{
				NumInstances = (int32)currMesh.m_instanceTransforms.size();

				DepthPassInstanceBuffer InstanceBuffer;

				for (int32 j = 0; j < NumInstances; ++j)
				{
					TTransform& transform = currMesh.m_instanceTransforms[j];
					matrix instancedWorld = transform.GetMatrix<true, true, true>();

					XMStoreFloat4x4(&InstanceBuffer.InstanceMatrix[j], (instancedWorld * view) * projection);
				}

				shader.WriteConstants("InstanceMatrix", (void*)&InstanceBuffer.InstanceMatrix, permutationIndex);
			}
		}

		//________________Bind shader data
		{
			shader.BindFrameBuffer(frameBuffer, permutationIndex);
			shader.SetShaderStages<EShaderStage::eVS>(context, permutationIndex);
			//HS DS
			shader.SetShaderStages<EShaderStage::ePS>(context, permutationIndex);

			shader.BindData(context, permutationIndex);

			context->OMSetDepthStencilState(dss, 0);
			context->OMSetRenderTargets(numRTVs, rtvs, dsv);
			context->RSSetViewports(1, &d3dview);
			context->IASetInputLayout(shader.m_inputlayout);
			context->RSSetState(renderer->GetRenderState((int32)model.m_rasterizationState));
			context->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(currMesh.m_topology));
		}


		//________________Draw shaders
		{
			UINT stride = vbuffer.m_info.m_stride;
			UINT offset = 0;

			context->IASetVertexBuffers(0, 1, &vbuffer.m_pGPUdata, &stride, &offset);

			bool bUsesIndexBuffer = currMesh.m_indexkey != -1;

			if (bUsesInstanceBuffer)
			{
				if (bUsesIndexBuffer)
				{
					TD3DBuffer ibuffer = renderer->GetIndexBuffer(currMesh.m_indexkey);
					context->IASetIndexBuffer(ibuffer.m_pGPUdata, DXGI_FORMAT_R32_UINT, 0);
					context->DrawIndexedInstanced(ibuffer.m_info.m_elementcount, NumInstances, 0, 0, 0);
				}
				else
				{
					context->DrawInstanced(vbuffer.m_info.m_elementcount, NumInstances, 0, 0);
				}
			}
			else
			{
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

	}
}