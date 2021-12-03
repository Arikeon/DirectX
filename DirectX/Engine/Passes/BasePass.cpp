#include "BasePass.h"
#include "StructCollection.h"
#include "BasePassPS.hlsl"
#include "BasePassVS.hlsl"
#include "FrameBuffer.h"

void TBasePass::RenderSceneMesh(CRenderer* renderer,
	std::vector<TObject>& objects,
	CCamera camera,
	TWindow window,
	float delta)
{
	ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();
	TShader& BasePassShader = renderer->GetShader(EShaderList::eBasePass);

	std::vector<ID3D11RenderTargetView*> GBufferRenderTargets;

	const int32 numRTVs = EGBufferKeys::eMax;
	check(numRTVs <= RTVSLOTMAX);

	for (int32 i = 0; i < numRTVs; ++i)
	{
		GBufferRenderTargets.push_back(renderer->GetGBufferRTV(i).m_rtv);
	}

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
			DrawMeshes(renderer, BasePassShader, camera, window, delta, model, GBufferRenderTargets);
		}
	}


	renderer->UnbindRTV();
	renderer->UnbindSRV(BasePassShader);
}

void TBasePass::RenderSkyBoxAndUI(CRenderer* renderer,
	TObject& Skybox,
	TDebugLines debuglines,
	CCamera camera,
	TWindow window,
	float delta)
{
	ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();
	TShader& BasePassShader = renderer->GetShader(EShaderList::eBasePass);

	std::vector<ID3D11RenderTargetView*> colorRenderTarget;

	colorRenderTarget.push_back(renderer->GetRenderTarget(ERenderTargetKey::eColor).m_rtv);

	{
		TModel& model = Skybox.m_models[0];
		DrawMeshes(renderer, BasePassShader, camera, window, delta, model, colorRenderTarget);
	}

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

	renderer->UnbindRTV();
	renderer->UnbindSRV(BasePassShader);
}


void TBasePass::DrawMeshes(CRenderer* renderer,
	TShader shader,
	CCamera camera,
	TWindow window,
	float delta,
	TModel model,
	std::vector<ID3D11RenderTargetView*> rtvs)
{
	ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();
	ID3D11DepthStencilView* dsv = renderer->GetGBufferDepth().m_dsv;
	ID3D11DepthStencilState* dss = renderer->GetDepthStencilState((int32)EDepthStencilStates::eBasePass).m_dss;

	FrameBuffer& frameBuffer = renderer->GetFrameBuffer();

	for (int i = 0; i < (int)model.m_meshes.size(); ++i)
	{
		TMesh& currMesh = model.m_meshes[i];

		if (!currMesh.m_bInitialized)
			continue;

		TD3DBuffer vbuffer;
		vbuffer = renderer->GetVertexBuffer(currMesh.m_vertexkey);

		D3D11_VIEWPORT& d3dview = renderer->GetView(EViews::eMain);

		bool bHasMaterial = currMesh.HasMaterial();
		bool bHasInstancedMaterial = currMesh.HasInstancedOverloardedMaterial();
		bool bUsesInstanceBuffer = currMesh.HasInstances();
		bool bHasDiffuseMap = false, bHasNormalMap = false;

		int32 NumInstances = (int32)currMesh.m_instances.m_instanceTransforms.size();
		int32 permutationIndex = 0;
		std::vector<PermutationValue> permutationKey;

		permutationKey.push_back({ "USE_GBUFFER", (int32)rtvs.size() > 1 });
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
			matrix world, view, projection;
			view = XMLoadFloat4x4(&frameBuffer.View);
			projection = XMLoadFloat4x4(&frameBuffer.Projection);

			if (bUsesInstanceBuffer)
			{
				float4x4 InstanceMatricies[MAX_INSTANCE];

				for (int32 j = 0; j < NumInstances; ++j)
				{
					TTransform& transform = currMesh.m_instances.m_instanceTransforms[j];
					matrix instancedWorld = transform.GetMatrix<true, true, true>();

					XMStoreFloat4x4(&InstanceMatricies[j], (instancedWorld * view) * projection);
				}

				shader.WriteConstants("InstancedWorldViewProjection", (void*)&InstanceMatricies, permutationIndex);
			}
			else
			{
				world = model.m_transform.GetMatrix<true, false, true>();

				float4x4 WorldViewProjection;
				XMStoreFloat4x4(&WorldViewProjection, (world * view) * projection);

				shader.WriteConstants("WorldViewProjection", (void*)&WorldViewProjection, permutationIndex);
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

			if (sampler.IsValid())
				shader.BindSampler(context, "s_Sampler", sampler.m_samplerstate, permutationIndex);

			if (bHasDiffuseMap)
			{
				TD3DTexture Diffuse = renderer->GetTexture(Material.m_textureDiffuse);
				if (Diffuse.IsValid())
					shader.BindTexture(context, "t_Diffuse", Diffuse.m_srv, permutationIndex);
			}

			if (bHasNormalMap)
			{
				TD3DTexture Normal = renderer->GetTexture(Material.m_textureNormal);
				if (Normal.IsValid())
					shader.BindTexture(context, "t_Normal", Normal.m_srv, permutationIndex);
			}


			if (bHasInstancedMaterial)
			{
				check((int32)currMesh.m_instances.m_instanceMaterials.size() == NumInstances);

				InstancedBasePassMaterial ConstantMaterial;

				for (int32 ins = 0; ins < NumInstances; ++ins)
				{
					TMaterial& InstancedMaterial = renderer->GetMaterial(currMesh.m_instances.m_instanceMaterials[ins]);

					ConstantMaterial.InstancedDiffuseColor[ins] = float4(InstancedMaterial.m_diffuseColor.x,
						InstancedMaterial.m_diffuseColor.y, InstancedMaterial.m_diffuseColor.z, 0.f);

					ConstantMaterial.InstancedSpecular[ins] = float4(InstancedMaterial.m_specularColor.x,
						InstancedMaterial.m_specularColor.y, InstancedMaterial.m_specularColor.z, 0.f);

					ConstantMaterial.InstancedRoughness[ins] = InstancedMaterial.m_roughness;
					ConstantMaterial.InstancedMetallic[ins] = InstancedMaterial.m_metallic;
				}

				shader.WriteConstants("InstancedDiffuseColor", (void*)&ConstantMaterial.InstancedDiffuseColor, permutationIndex);
				shader.WriteConstants("InstancedSpecular", (void*)&ConstantMaterial.InstancedSpecular, permutationIndex);
				shader.WriteConstants("InstancedRoughness", (void*)&ConstantMaterial.InstancedRoughness, permutationIndex);
				shader.WriteConstants("InstancedMetallic", (void*)&ConstantMaterial.InstancedMetallic, permutationIndex);
			}
			else
			{
				BasePassMaterial ConstantMaterial = {};
				ConstantMaterial.DiffuseColor = Material.m_diffuseColor;
				//ConstantMaterial.Specular = Material.m_specularColor;
				ConstantMaterial.Roughness = Material.m_roughness;
				ConstantMaterial.Metallic = Material.m_metallic;
				shader.WriteConstants("DiffuseColor", (void*)&ConstantMaterial.DiffuseColor, permutationIndex);
				//shader.WriteConstants("Specular", (void*)&ConstantMaterial0.Specular, permutationIndex);
				shader.WriteConstants("Roughness", (void*)&ConstantMaterial.Roughness, permutationIndex);
				shader.WriteConstants("Metallic", (void*)&ConstantMaterial.Metallic, permutationIndex);
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
			context->OMSetRenderTargets((UINT)rtvs.size(), &rtvs[0], dsv);
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
