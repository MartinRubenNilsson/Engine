#pragma once

class RenderTarget
{
public:
	virtual ~RenderTarget() = default;

	void SetRenderTarget();
	void ClearRenderTarget(const float aColor[4]);

protected:
	ComPtr<ID3D11RenderTargetView> myRenderTargetView;
};

