#include "DeferredLightingPass.h"
#include "ScreenQuadStructs.h"
#include "DeferredlightingPS.hlsl"

void TDeferredLightingPass::Render(CRenderer* renderer,
	std::vector<TLight> Lights,
	TObject& ScreenQuadObject,
	CCamera camera,
	float delta)
{
	check(ScreenQuadObject.m_models.size() == 1);

	TShader DeferredLightingPS = renderer->GetShader(EShaderList::eDeferredLighting);
	TShader ScreenQuadVS = renderer->GetShader(EShaderList::eScreenQuad);

	DrawScreenQuad(renderer, Lights, DeferredLightingPS, ScreenQuadVS, ScreenQuadObject.m_models[0], camera, delta);

	renderer->UnbindRTV();
	renderer->UnbindSRV(ScreenQuadVS);
	renderer->UnbindSRV(DeferredLightingPS);
}

void TDeferredLightingPass::DrawScreenQuad(CRenderer* renderer,
	std::vector<TLight> Lights,
	TShader DeferredLightingPS,
	TShader ScreenQuadVS,
	TModel& ScreenQuadModel,
	CCamera camera,
	float delta)
{
	ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();

	//render to back buffer for now
	ID3D11RenderTargetView* backbuffer = renderer->GetRenderTarget(0).m_rtv;

	int32 permutationIndex = DeferredLightingPS.m_permutations.GetShaderWithPermutation({
		{"LIGHT_TYPE_DIRECTIONAL",	(int32)0},
		{"LIGHT_TYPE_POINT",		(int32)0},
		{"LIGHT_TYPE_SPOT",			(int32)0},
		{"SHADOWED",				(int32)0}});



	//Gather Lights
	std::vector<TDirectionalLight> DirectionalLights;
	std::vector<TPointLight> PointLights;
	std::vector<TSpotLight> SpotLights;

	{
		for (int32 i = 0; i < (int32)Lights.size(); ++i)
		{
			ELightType::Type type = Lights[i].m_lightType;

			switch (type)
			{
			case ELightType::eDirectional:
			{
				TDirectionalLight light = static_cast<TDirectionalLight&>(Lights[i]);
				DirectionalLights.push_back(light);
			}
			break;
			case ELightType::ePoint:
			{
				TPointLight light = static_cast<TPointLight&>(Lights[i]);
				PointLights.push_back(light);
			}
			break;
			case ELightType::eSpot:
			{
				TSpotLight light = static_cast<TSpotLight&>(Lights[i]);
				SpotLights.push_back(light);
			}
			break;
			}
		}

	}

	check(DirectionalLights.size() <= 1);

	LightBuffer lightBuffer = {};

	//Push light buffer data
	{
		if ((int32)DirectionalLights.size() == 1)
		{

			//Directional
			TDirectionalLight& Directional = DirectionalLights[0];

			lightBuffer.DirectionalColor = float4(
				Directional.m_color.x,
				Directional.m_color.y,
				Directional.m_color.z,
				0.f);

			lightBuffer.DirectionalPositionAndIntensity = float4(
				Directional.m_position.x,
				Directional.m_position.y,
				Directional.m_position.z,
				Directional.m_intensity);

			lightBuffer.DirectionalDirection = DirectionalLights[0].m_direction;
		}

		//Point
		for (int32 i = 0; i < (int32)PointLights.size(); ++i)
		{
			TPointLight& Point = PointLights[i];


		}


		DeferredLightingPS.WriteConstants("DirectionalColor",				(void*)&lightBuffer.DirectionalColor, permutationIndex);
		DeferredLightingPS.WriteConstants("DirectionalPositionAndIntensity",(void*)&lightBuffer.DirectionalPositionAndIntensity, permutationIndex);
		DeferredLightingPS.WriteConstants("DirectionalDirection",			(void*)&lightBuffer.DirectionalDirection, permutationIndex);
	}

	//TEMP
	DeferredBuffer deferredBuffer;
	{
		XMStoreFloat4(&deferredBuffer.CameraPosition, camera.m_pos);
		DeferredLightingPS.WriteConstants("CameraPosition", (void*)&deferredBuffer.CameraPosition);
	}

	//set Gbuffer and samplers
	{
		TD3DSampler sampler = renderer->GetSampler((SamplerID)EStaticSamplerKey::eDefault);
		DeferredLightingPS.SetSamplerState<EShaderStage::ePS>(context, sampler.m_samplerstate);

		TD3DTexture DiffuseTex = renderer->GetTexture(
			renderer->GetGBufferRTV(EGBufferKeys::eBaseColor).m_textureid);

		TD3DTexture WorldNormalTex = renderer->GetTexture(
			renderer->GetGBufferRTV(EGBufferKeys::eWorldNormal).m_textureid);

		TD3DTexture RoughnessTex = renderer->GetTexture(
			renderer->GetGBufferRTV(EGBufferKeys::eRoughness).m_textureid);

		TD3DTexture MetallicTex = renderer->GetTexture(
			renderer->GetGBufferRTV(EGBufferKeys::eMetallic).m_textureid);

		if (DiffuseTex.IsValid())
			DeferredLightingPS.SetShaderResource<EShaderStage::ePS, 0>(context, DiffuseTex.m_srv);

		if (WorldNormalTex.IsValid())
			DeferredLightingPS.SetShaderResource<EShaderStage::ePS, 1>(context, WorldNormalTex.m_srv);

		if (WorldNormalTex.IsValid())
			DeferredLightingPS.SetShaderResource<EShaderStage::ePS, 2>(context, WorldNormalTex.m_srv);

		if (MetallicTex.IsValid())
			DeferredLightingPS.SetShaderResource<EShaderStage::ePS, 3>(context, MetallicTex.m_srv);
	}
	
	TMesh& ScreenQuadMesh = ScreenQuadModel.m_meshes[0];

	//Bind Shader Data
	{
		ScreenQuadVS.SetShaderStages(context);
		DeferredLightingPS.SetShaderStages(context);
		ScreenQuadVS.BindData(context);
		DeferredLightingPS.BindData(context);

		D3D11_VIEWPORT& d3dview = renderer->GetView(EViews::eMain);
		TMesh& ScreenQuadMesh = ScreenQuadModel.m_meshes[0];
		check(ScreenQuadMesh.m_bInitialized);
		context->OMSetRenderTargets(1, &backbuffer, nullptr); //no depth
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
}