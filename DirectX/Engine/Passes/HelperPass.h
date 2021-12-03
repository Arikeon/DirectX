#pragma once
#include "Pass.h"

struct THelperPass : public TPass
{
	void CopyBuffer(CRenderer* renderer,
		TD3DRenderTarget target,
		TD3DRenderTarget destination);

	void CopyBuffer(CRenderer* renderer,
		TD3DTexture target,
		TD3DRenderTarget destination);

protected:
	void CopyBufferInner(CRenderer* renderer,
		TShader ScreenQuadVS,
		TShader HelperCopyPS,
		TD3DTexture target,
		TD3DRenderTarget destination);
};