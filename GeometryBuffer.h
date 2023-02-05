#pragma once

class GeometryBuffer
{
public:
	GeometryBuffer(unsigned aWidth, unsigned aHeight);

	void ClearRenderTargets(const Color& aColor = { 0.f, 0.f, 0.f, 0.f });
	void SetRenderTargets(ID3D11DepthStencilView* aDepthStencil = nullptr) const; // also sets viewports

	void VSSetShaderResources(unsigned aStartSlot) const;
	void PSSetShaderResources(unsigned aStartSlot) const;

	operator bool() const { return SUCCEEDED(myResult); }

private:
	static constexpr std::array ourFormats =
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT, // World position
		DXGI_FORMAT_R32G32B32A32_FLOAT, // World normal
	};

	static constexpr UINT ourBufferCount = static_cast<UINT>(ourFormats.size());
	static_assert(ourBufferCount <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

	HRESULT myResult;
	ComPtr<ID3D11Texture2D> myTextures[ourBufferCount];
	ComPtr<ID3D11RenderTargetView> myRenderTargets[ourBufferCount];
	ComPtr<ID3D11ShaderResourceView> myShaderResources[ourBufferCount];
	unsigned myWidth;
	unsigned myHeight;
};

