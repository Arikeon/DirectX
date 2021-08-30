#include "DeferredLightingPass.h"
#include "ScreenQuadStructs.h"

void TDeferredLightingPass::Render(CRenderer* renderer, TObject& ScreenQuadObject, float delta)
{
	check(ScreenQuadObject.m_models.size() == 1);

	TShader DeferredLightingPS = renderer->GetShader(EShaderList::eDeferredLighting);
	TShader ScreenQuadVS = renderer->GetShader(EShaderList::eScreenQuad);

	renderer->UnbindRTV();
	renderer->UnbindSRV(ScreenQuadVS);
	renderer->UnbindSRV(DeferredLightingPS);

	//TODO batch unshadowed lights
	DrawScreenQuad(renderer, DeferredLightingPS, ScreenQuadVS, ScreenQuadObject.m_models[0], delta);

	//TODO draw call per shadowed lights - could optimize with resource tables on DX12
}

void TDeferredLightingPass::DrawScreenQuad(CRenderer* renderer,
	TShader DeferredLightingPS,
	TShader ScreenQuadVS,
	TModel& ScreenQuadModel,
	float delta)
{
	ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();

	//render to back buffer for now
	ID3D11RenderTargetView* backbuffer = renderer->GetRenderTarget(0).m_rtv;

	ScreenQuadVS.SetShaderStages(context);

	int32 permutationIndex = DeferredLightingPS.m_permutations.GetShaderWithPermutation({
		{"LIGHT_TYPE_DIRECTIONAL",	(int32)0},
		{"LIGHT_TYPE_POINT",		(int32)0},
		{"LIGHT_TYPE_SPOT",			(int32)0}});

	DeferredLightingPS.SetShaderStages(context);

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