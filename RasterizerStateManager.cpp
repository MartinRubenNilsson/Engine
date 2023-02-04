#include "pch.h"
#include "RasterizerStateManager.h"

RasterizerStateManager::RasterizerStateManager()
	: myDesc
	{
		D3D11_FILL_SOLID, // FillMode
		D3D11_CULL_BACK , // CullMode
		FALSE, // FrontCounterClockwise
		0, // DepthBias
		0.f, // DepthBiasClamp
		0.f, // SlopeScaledDepthBias
		TRUE, // DepthClipEnable
		FALSE, // ScissorEnable
		FALSE, // MultisampleEnable
		FALSE // AntialiasedLineEnable
	}
	, myStates{}
{
}

void RasterizerStateManager::SetState()
{
	auto itr = myStates.find(myDesc);

	if (itr == myStates.end())
	{
		assert(myStates.size() < 4096 && "Cannot create more than 4096 unique rasterizer state objects");

		ComPtr<ID3D11RasterizerState> state{};
		DX11_DEVICE->CreateRasterizerState(&myDesc, &state);
		itr = myStates.emplace(myDesc, state).first;
	}

	DX11_CONTEXT->RSSetState(itr->second.Get());
}

void RasterizerStateManager::GetState()
{
	ComPtr<ID3D11RasterizerState> state{};
	DX11_CONTEXT->RSGetState(&state);
	if (state)
		state->GetDesc(&myDesc);
}

void RasterizerStateManager::SetFillMode(FillMode aMode)
{
	myDesc.FillMode = static_cast<D3D11_FILL_MODE>(aMode);
}

FillMode RasterizerStateManager::GetFillMode() const
{
	return static_cast<FillMode>(myDesc.FillMode);
}

void ImGui::RasterizerDesc(D3D11_RASTERIZER_DESC& aDesc)
{
	{
		int fillMode = aDesc.FillMode - D3D11_FILL_WIREFRAME;
		Combo("Fill Mode", &fillMode, "Wireframe\0Solid\0\0");
		aDesc.FillMode = static_cast<D3D11_FILL_MODE>(fillMode + D3D11_FILL_WIREFRAME);
	}

	{
		int cullMode = aDesc.CullMode - D3D11_CULL_NONE;
		Combo("Cull Mode", &cullMode, "None\0Front\0Back\0\0");
		aDesc.CullMode = static_cast<D3D11_CULL_MODE>(cullMode + D3D11_CULL_NONE);
	}

	ImGui::Checkbox("Front Counter Clockwise", (bool*) & aDesc.FrontCounterClockwise);

	ImGui::InputInt("Depth Bias", &aDesc.DepthBias);
	ImGui::InputFloat("Depth Bias Clamp", &aDesc.DepthBiasClamp);
	ImGui::InputFloat("Slope Scaled Depth Bias", &aDesc.SlopeScaledDepthBias);

	ImGui::Checkbox("Depth Clip Enable", (bool*)&aDesc.DepthClipEnable);
	ImGui::Checkbox("Scissor Enable", (bool*)&aDesc.ScissorEnable);
	ImGui::Checkbox("Multisample Enable", (bool*)&aDesc.MultisampleEnable);
	ImGui::Checkbox("Antialiased Line Enable", (bool*)&aDesc.AntialiasedLineEnable);
}

void ImGui::RasterizerStateManager(class RasterizerStateManager& aMgr)
{
	{
		D3D11_RASTERIZER_DESC desc = aMgr.GetDesc();
		RasterizerDesc(desc);
		aMgr.SetDesc(desc);
	}

	if (Button("Set State"))
		aMgr.SetState();
	SameLine();
	if (Button("Get State"))
		aMgr.GetState();
}
