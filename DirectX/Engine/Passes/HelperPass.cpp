#include "HelperPass.h"

void THelperPass::CopyBuffer(CRenderer* renderer,
	TD3DRenderTarget target,
	TD3DRenderTarget destination)
{
	TD3DTexture targetTexture = renderer->GetTexture(target.m_textureid);
	CopyBuffer(renderer, targetTexture, destination);
}

void THelperPass::CopyBuffer(CRenderer* renderer,
	TD3DTexture target,
	TD3DRenderTarget destination)
{
	check(target.IsValid() && destination.IsValid());

	TShader ScreenQuadVS = renderer->GetShader(EShaderList::eScreenQuad);
	TShader HelperPS = renderer->GetShader(EShaderList::eHelper);

	CopyBufferInner(renderer, ScreenQuadVS, HelperPS, target, destination);

	renderer->UnbindRTV();
	renderer->UnbindSRV(ScreenQuadVS);
	renderer->UnbindSRV(HelperPS);
}

void THelperPass::CopyBufferInner(CRenderer* renderer,
	TShader ScreenQuadVS,
	TShader HelperCopyPS,
	TD3DTexture target,
	TD3DRenderTarget destination)
{
	ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();

	TMesh& ScreenQuadMesh = GetScreenQuad().m_models[0].m_meshes[0];
	check(ScreenQuadMesh.m_bInitialized);


	//Bind Shader Resources
	{
		TD3DSampler sampler = renderer->GetSampler((SamplerID)EStaticSamplerKey::eDefault);
		HelperCopyPS.BindTexture(context, "t_Target", target.m_srv);
		HelperCopyPS.BindSampler(context, "s_Sampler", sampler.m_samplerstate);

		ScreenQuadVS.SetShaderStages(context);
		HelperCopyPS.SetShaderStages(context);

		ScreenQuadVS.BindData(context);
		HelperCopyPS.BindData(context);
	}

	//Bind Shader Data
	{
		D3D11_VIEWPORT& d3dview = renderer->GetView(EViews::eMain);
		context->OMSetRenderTargets(1, &destination.m_rtv, nullptr); //no depth
		context->RSSetViewports(1, &d3dview);
		context->IASetInputLayout(ScreenQuadVS.m_inputlayout);
		context->RSSetState(renderer->GetRenderState((int32)ERasterizerStates::eFillSolidCullNone));
		context->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(ScreenQuadMesh.m_topology));
	}

	//Draw Quad
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
