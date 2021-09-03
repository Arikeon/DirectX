#include "BasePass.h"
#include "BasePassStructs.h"
#include "BasePassPS.hlsl"
#include "BasePassVS.hlsl"


void TBasePass::Render(CRenderer* renderer,
	std::vector<TObject>& objects,
	TDebugLines debuglines,
	CCamera camera,
	TWindow window,
	float delta)
{
	ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();

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

		TShader& BasePassShader = renderer->GetShader(EShaderList::eBasePass);

		for (int i = 0; i < nummodels; ++i)
		{
			TModel& model = models[i];
			DrawMeshes(renderer, BasePassShader, camera, window, delta, model);
		}

		renderer->UnbindRTV();
		renderer->UnbindSRV(BasePassShader);

		//Render this AFTER scene has been rendered - make new draw pass
#if 0
		//Draw debug lines
		if (debuglines.m_meshes.size() > 0)
		{
			TD3DBuffer vbuffer = renderer->GetVertexBuffer(debuglines.m_meshes[0].m_vertexkey);
			D3D11_MAPPED_SUBRESOURCE subresourcemap;
			context->Map(vbuffer.m_pGPUdata, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &subresourcemap);
			memcpy(subresourcemap.pData, &debuglines.m_debuglines[0], sizeof(DebugLinesInVS) * debuglines.GetLineCount());
			context->Unmap(vbuffer.m_pGPUdata, (UINT)0);

			DrawMeshes(renderer, renderer->GetShader(EShaderList::eDebugBasePass), camera, window, delta, debuglines);
		}
#endif
	}
}


void TBasePass::DrawMeshes(CRenderer* renderer,
	TShader shader,
	CCamera camera,
	TWindow window,
	float delta,
	TModel model)
{
	ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();
	ID3D11DepthStencilView* dsv = renderer->GetDepthTarget(0).m_dsv;

	const int32 numRTVs = EGBufferKeys::eMax;
	ID3D11RenderTargetView* rtvs[numRTVs];

	check(numRTVs <= RTVSLOTMAX);
	for (int32 i = 0; i < numRTVs; ++i)
	{
		rtvs[i] = renderer->GetGBufferRTV(i).m_rtv;
	}

	for (int i = 0; i < (int)model.m_meshes.size(); ++i)
	{
		TMesh& currMesh = model.m_meshes[i];

		if (!currMesh.m_bInitialized)
			continue;

		TD3DBuffer vbuffer;
		vbuffer = renderer->GetVertexBuffer(currMesh.m_vertexkey);

		D3D11_VIEWPORT& d3dview = renderer->GetView(EViews::eMain);
		const float aspectratio = static_cast<float>(window.m_width) / static_cast<float>(window.m_height);

		bool bHasMaterial = currMesh.HasMaterial();
		bool bUsesInstanceBuffer = currMesh.HasInstances();
		bool bHasDiffuseMap = false, bHasNormalMap = false;

		int32 NumInstances = 0;
		int32 permutationIndex = 0;
		std::vector<PermutationValue> permutationKey;

		permutationKey.push_back({ "USE_INSTANCING", (int32)bUsesInstanceBuffer });

		if (bHasMaterial)
		{
			TMaterial& Material = renderer->GetMaterial(currMesh.m_materialKey);

			bHasDiffuseMap = Material.m_textureDiffuse != -1;
			bHasNormalMap = Material.m_textureNormal != -1;
		}

		permutationKey.push_back({ "USE_TEXTURE_DIFFUSE" , (int32)bHasDiffuseMap });
		permutationKey.push_back({ "USE_TEXTURE_Normal" , (int32)bHasNormalMap });

		permutationIndex = shader.m_permutations.GetShaderWithPermutation(permutationKey);


		//________________Map Vertex & Instance data
		{
			//World View Proj buffer
			BasePassWVP WVP;
			XMStoreFloat4x4(&WVP.World, model.m_transform.GetMatrix<true, true, true>());
			WVP.World._44 = 1.f;
			WVP.View = camera.m_cameramatrix;


			//Setup projection per frame
			XMStoreFloat4x4(&WVP.Proj, XMMatrixPerspectiveFovLH(window.FOV, aspectratio, 0.001f, FRUSTRUM_FAR_PLANE));
			shader.WriteConstants("World", (void*)&WVP.World, permutationIndex);
			shader.WriteConstants("View", (void*)&WVP.View, permutationIndex);
			shader.WriteConstants("Proj", (void*)&WVP.Proj, permutationIndex);

			if (bUsesInstanceBuffer)
			{
				NumInstances = (int32)currMesh.m_instanceTransforms.size();

				BasePassInstanceBuffer InstanceBufferStruct = {};
				for (int32 j = 0; j < NumInstances; ++j)
				{
					TTransform& transform = currMesh.m_instanceTransforms[j];
					XMStoreFloat4x4(&InstanceBufferStruct.InstanceMatrix[j], transform.GetMatrix<true, true, true>());
				}

				shader.WriteConstants("InstanceMatrix", (void*)&InstanceBufferStruct.InstanceMatrix, permutationIndex);
			}
		}



		//________________Map Pixel Shader data
		if (bHasMaterial)
		{
			TMaterial& Material = renderer->GetMaterial(currMesh.m_materialKey);
			TD3DSampler sampler;

			if (Material.bHasSamplerOverride())
			{
				sampler = renderer->GetSampler(Material.m_samplerOverride);
			}
			else
			{
				sampler = renderer->GetSampler((SamplerID)EStaticSamplerKey::eDefault);
			}

			shader.SetSamplerState<EShaderStage::ePS>(context, sampler.m_samplerstate);

			if (bHasDiffuseMap)
			{
				TD3DTexture Diffuse = renderer->GetTexture(Material.m_textureDiffuse);
				shader.SetShaderResource<EShaderStage::ePS, 0>(context, Diffuse.m_srv);
			}

			if (bHasNormalMap)
			{
				TD3DTexture Normal = renderer->GetTexture(Material.m_textureNormal);
				shader.SetShaderResource<EShaderStage::ePS, 1>(context, Normal.m_srv);
			}

			BasePassMaterial ConstantMaterial = {};
			ConstantMaterial.DiffuseColor = float4(Material.m_diffuseColor.x, Material.m_diffuseColor.y, Material.m_diffuseColor.z, 0.f);
			ConstantMaterial.Roughness = Material.m_roughness;
			ConstantMaterial.Metallic = Material.m_metallic;
			shader.WriteConstants("DiffuseColor", (void*)&ConstantMaterial.DiffuseColor, permutationIndex);
		}



		//________________Bind shader data
		{
			shader.SetShaderStages<EShaderStage::eVS>(context, permutationIndex);
			//HS DS
			shader.SetShaderStages<EShaderStage::ePS>(context, permutationIndex);

			shader.BindData(context, permutationIndex);

			context->OMSetRenderTargets(numRTVs, rtvs, dsv);
			context->RSSetViewports(1, &d3dview);
			context->IASetInputLayout(shader.m_inputlayout);
			//TODO add forced rasterization state for wireframe view mode
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
