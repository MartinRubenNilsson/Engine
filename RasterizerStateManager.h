#pragma once

template <>
struct std::hash<D3D11_RASTERIZER_DESC>
{
	size_t operator()(const D3D11_RASTERIZER_DESC& aDesc) const
	{
		return std::hash<string_view>{}({ reinterpret_cast<const char*>(&aDesc), sizeof(aDesc) });
	}
};

template <>
struct std::equal_to<D3D11_RASTERIZER_DESC>
{
	bool operator()(const D3D11_RASTERIZER_DESC& lhs, const D3D11_RASTERIZER_DESC& rhs) const
	{
		return std::memcmp(&lhs, &rhs, sizeof(D3D11_RASTERIZER_DESC)) == 0;
	}
};

enum class FillMode
{
	Wireframe = 2,
	Solid = 3
};

class RasterizerStateManager : public Singleton<RasterizerStateManager>
{
public:
	RasterizerStateManager();

	void SetState();
	void GetState();

	void SetFillMode(FillMode);
	FillMode GetFillMode() const;

	// todo: other members

	void SetDesc(const D3D11_RASTERIZER_DESC& aDesc) { myDesc = aDesc; }
	D3D11_RASTERIZER_DESC GetDesc() const { return myDesc; }

private:
	D3D11_RASTERIZER_DESC myDesc;
	std::unordered_map<D3D11_RASTERIZER_DESC, ComPtr<ID3D11RasterizerState>> myStates;
};

namespace ImGui
{
	void RasterizerDesc(D3D11_RASTERIZER_DESC& aDesc);
	void RasterizerStateManager(class RasterizerStateManager& aMgr);
}

#define DX11_RASTERIZER RasterizerStateManager::Get()

