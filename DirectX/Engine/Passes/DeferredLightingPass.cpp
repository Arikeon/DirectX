#include "DeferredLightingPass.h"
#include "StructCollection.h"
#include "DeferredlightingPS.hlsl"

void TDeferredLightingPass::Render(CRenderer* renderer,
	std::vector<TLight> Lights,
	CCamera camera,
	TWindow window,
	float delta)
{
	TShader ScreenQuadVS = renderer->GetShader(EShaderList::eScreenQuad);
	TShader DeferredLightingPS = renderer->GetShader(EShaderList::eDeferredLighting);

	DrawScreenQuad(renderer, Lights, DeferredLightingPS, ScreenQuadVS, camera, window, delta);
}

void TDeferredLightingPass::DrawScreenQuad(CRenderer* renderer,
	std::vector<TLight> Lights,
	TShader DeferredLightingPS,
	TShader ScreenQuadVS,
	CCamera camera,
	TWindow window,
	float delta)
{
	ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();
	//render to back buffer for now
	ID3D11RenderTargetView* ColorRTV = renderer->GetRenderTarget(ERenderTargetKey::eColor).m_rtv;

	FrameBuffer& frameBuffer = renderer->GetFrameBuffer();

	//Gather Lights
	std::array<std::vector<TLight>, ELightType::eMax> GatheredLights;
	std::array<std::vector<TLight>, ELightType::eMax> GatheredLightsShadowed;
	int32 numLights = 0;

	for (int32 i = 0; i < (int32)Lights.size(); ++i)
	{
		ELightType::Type type = Lights[i].m_lightType;

		//also check visibility
		switch (type)
		{
		case ELightType::eDirectional:
		{
			TDirectionalLight light = static_cast<TDirectionalLight&>(Lights[i]);
			light.m_shadowed ?
				GatheredLightsShadowed[ELightType::eDirectional].push_back(light) : GatheredLights[ELightType::eDirectional].push_back(light);
			++numLights;
		}
		break;
		case ELightType::ePoint:
		{
			TPointLight light = static_cast<TPointLight&>(Lights[i]);
			light.m_shadowed ?
				GatheredLightsShadowed[ELightType::ePoint].push_back(light) : GatheredLights[ELightType::ePoint].push_back(light);
			++numLights;
		}
		break;
		case ELightType::eSpot:
		{
			TSpotLight light = static_cast<TSpotLight&>(Lights[i]);
			light.m_shadowed ?
				GatheredLightsShadowed[ELightType::eSpot].push_back(light) : GatheredLights[ELightType::eSpot].push_back(light);
			++numLights;
		}
		break;
		}
	}

	if (numLights == 0)
		return;


	LightBuffer lightBuffer = {};
	//Create batched buffer data
	if (1)
	{
		lightBuffer.AmbientStrength = .04f;

		if ((int32)GatheredLights[ELightType::eDirectional].size() > 0)
		{
			//Directional
			TDirectionalLight& Directional = static_cast<TDirectionalLight&>(GatheredLights[ELightType::eDirectional][0]);

			LightData& DirLData = lightBuffer.LData[0];

			DirLData.LColor = float4(Directional.m_color);
			DirLData.LPositionAndIntensity = float4(Directional.m_position, Directional.m_intensity);
		}

		//Point
		//	lightBuffer.PointLightCount = PointLights.size();
		//for (int32 i = 0; i < (int32)PointLights.size(); ++i)
		//{
		//	TPointLight& Point = PointLights[i];
		//}
	}


	//init Gbuffer and samplers
	TD3DSampler sampler = renderer->GetSampler((SamplerID)EStaticSamplerKey::eDefault);
	DeferredLightingPS.BindSampler(context, "s_Sampler", sampler.m_samplerstate);

	TD3DTexture DiffuseTex = renderer->GetTexture(
		renderer->GetGBufferRTV(EGBufferKeys::eBaseColor).m_textureid);

	TD3DTexture WorldNormalTex = renderer->GetTexture(
		renderer->GetGBufferRTV(EGBufferKeys::eWorldNormal).m_textureid);

	TD3DTexture RoughnessMetallicSpecularTex = renderer->GetTexture(
		renderer->GetGBufferRTV(EGBufferKeys::eRoughnessMetallicSpecular).m_textureid);

	TD3DTexture DepthTexture = renderer->GetTexture(renderer->GetGBufferDepth().m_textureid);

	check(DiffuseTex.IsValid()
		&& WorldNormalTex.IsValid()
		&& RoughnessMetallicSpecularTex.IsValid()
		&& DepthTexture.IsValid());

	TMesh& ScreenQuadMesh = GetScreenQuad().m_models[0].m_meshes[0];

	for (int32 i = 0; i < ELightType::eMax; ++i)
	{
		if ((int32)GatheredLights[i].size() <= 0)
			continue;

		int32 permutationIndex = DeferredLightingPS.m_permutations.GetShaderWithPermutation({
			{"LIGHT_TYPE_DIRECTIONAL",	(int32)i == (int32)ELightType::eDirectional ? 1 : 0},
			{"LIGHT_TYPE_POINT",		(int32)i == (int32)ELightType::ePoint ? 1 : 0},
			{"LIGHT_TYPE_SPOT",			(int32)i == (int32)ELightType::eSpot ? 1 : 0},
			{"SHADOWED",				(int32)0} });

		//Bind Shader Data
		{
			DeferredLightingPS.WriteConstants("LData", (void*)&lightBuffer.LData, permutationIndex);
			DeferredLightingPS.WriteConstants("AmbientStrength", (void*)&lightBuffer.AmbientStrength, permutationIndex);

			DeferredLightingPS.BindTexture(context, "t_Diffuse", DiffuseTex.m_srv, permutationIndex);
			DeferredLightingPS.BindTexture(context, "t_Depth", DepthTexture.m_srv, permutationIndex);
			DeferredLightingPS.BindTexture(context, "t_WorldNormal", WorldNormalTex.m_srv, permutationIndex);
			DeferredLightingPS.BindTexture(context, "t_RoughnessMetallicSpecular", RoughnessMetallicSpecularTex.m_srv, permutationIndex);

			ScreenQuadVS.BindFrameBuffer(frameBuffer);
			DeferredLightingPS.BindFrameBuffer(frameBuffer, permutationIndex);

			ScreenQuadVS.SetShaderStages(context);
			DeferredLightingPS.SetShaderStages(context, permutationIndex);

			ScreenQuadVS.BindData(context);
			DeferredLightingPS.BindData(context, permutationIndex);

			D3D11_VIEWPORT& d3dview = renderer->GetView(EViews::eMain);
			TMesh& ScreenQuadMesh = GetScreenQuad().m_models[0].m_meshes[0];
			check(ScreenQuadMesh.m_bInitialized);
			context->OMSetRenderTargets(1, &ColorRTV, nullptr); //no depth
			context->RSSetViewports(1, &d3dview);
			context->IASetInputLayout(ScreenQuadVS.m_inputlayout);
			context->RSSetState(renderer->GetRenderState((int32)ERasterizerStates::eFillSolidCullNone));
			context->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(ScreenQuadMesh.m_topology));
		}

		//Draw quad
		{
			TD3DBuffer vbuffer;
			vbuffer = renderer->GetVertexBuffer(ScreenQuadMesh.m_vertexkey);
			TD3DBuffer ibuffer;
			ibuffer = renderer->GetIndexBuffer(ScreenQuadMesh.m_indexkey);


			UINT stride = vbuffer.m_info.m_stride;
			UINT offset = 0;

			context->IASetVertexBuffers(0, 1, &vbuffer.m_pGPUdata, &stride, &offset);
			context->IASetIndexBuffer(ibuffer.m_pGPUdata, DXGI_FORMAT_R32_UINT, 0);
			context->DrawIndexed(ibuffer.m_info.m_elementcount, 0, 0);
		}

		renderer->UnbindRTV();
		renderer->UnbindSRV(ScreenQuadVS);
		renderer->UnbindSRV(DeferredLightingPS);
	}
}