#pragma once

class GeometryBuffer
{
public:
	GeometryBuffer(unsigned aWidth, unsigned aHeight);

	void SetRenderTargets(ID3D11DepthStencilView* aDepth = nullptr) const;
	void VSSetShaderResources(unsigned aStartSlot) const;
	void PSSetShaderResources(unsigned aStartSlot) const;

	operator bool() const { return SUCCEEDED(myResult); }

private:
	static constexpr std::array ourFormats =
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT, // Position
		DXGI_FORMAT_R32G32B32A32_FLOAT, // Normal
	};

	static_assert(ourFormats.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

	ComPtr<ID3D11Texture2D> myTextures[ourFormats.size()];
	ComPtr<ID3D11RenderTargetView> myRenderTargets[ourFormats.size()];
	ComPtr<ID3D11ShaderResourceView> myShaderResources[ourFormats.size()];
	HRESULT myResult;
};

