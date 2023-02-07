#include "pch.h"
#include "StateManager.h"

void StateManager::SetRasterizerState(const D3D11_RASTERIZER_DESC& aDesc)
{
	auto& state = myRasterizerStates[aDesc];
	if (!state)
	{
		assert(myRasterizerStates.size() <= D3D11_REQ_RASTERIZER_OBJECT_COUNT_PER_DEVICE);
		DX11_DEVICE->CreateRasterizerState(&aDesc, &state);
	}
	DX11_CONTEXT->RSSetState(state.Get());
}

void StateManager::GetRasterizerState(D3D11_RASTERIZER_DESC& aDesc) const
{
	ComPtr<ID3D11RasterizerState> state{};
	DX11_CONTEXT->RSGetState(&state);
	if (state)
		state->GetDesc(&aDesc);
}

void StateManager::SetSamplerStates(UINT aStartSlot, std::span<const D3D11_SAMPLER_DESC> someDescs)
{
	assert(aStartSlot < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	std::vector<ID3D11SamplerState*> states{};
	for (size_t i = 0; i < someDescs.size(); ++i)
	{
		auto& state = mySamplerStates[someDescs[i]];
		if (state)
			continue;
		assert(mySamplerStates.size() <= D3D11_REQ_SAMPLER_OBJECT_COUNT_PER_DEVICE);
		DX11_DEVICE->CreateSamplerState(&someDescs[i], &state);
		states.push_back(state.Get());
	}
	DX11_CONTEXT->PSSetSamplers(aStartSlot, (UINT)states.size(), states.data());
}

void StateManager::GetSamplerStates(UINT aStartSlot, std::span<D3D11_SAMPLER_DESC> someDescs) const
{
	assert(aStartSlot < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	assert(aStartSlot + someDescs.size() <= D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	std::vector<ID3D11SamplerState*> states{ someDescs.size(), nullptr };
	DX11_CONTEXT->PSGetSamplers(aStartSlot, (UINT)states.size(), states.data());
	for (size_t i = 0; i < someDescs.size(); ++i)
	{
		if (states[i])
			states[i]->GetDesc(&someDescs[i]);
	}

}

void StateManager::SetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& aDesc, UINT aStencilRef)
{
	auto& state = myDepthStencilStates[aDesc];
	if (!state)
	{
		assert(myDepthStencilStates.size() <= D3D11_REQ_DEPTH_STENCIL_OBJECT_COUNT_PER_DEVICE);
		DX11_DEVICE->CreateDepthStencilState(&aDesc, &state);
	}
	DX11_CONTEXT->OMSetDepthStencilState(state.Get(), aStencilRef);
}

void StateManager::GetDepthStencilState(D3D11_DEPTH_STENCIL_DESC& aDesc, UINT& aStencilRef) const
{
	ComPtr<ID3D11DepthStencilState> state{};
	DX11_CONTEXT->OMGetDepthStencilState(&state, &aStencilRef);
	if (state)
		state->GetDesc(&aDesc);
}

void StateManager::SetBlendState(const D3D11_BLEND_DESC& aDesc, const FLOAT aBlendFactor[4], UINT aSampleMask)
{
	auto& state = myBlendStates[aDesc];
	if (!state)
	{
		assert(myBlendStates.size() <= D3D11_REQ_BLEND_OBJECT_COUNT_PER_DEVICE);
		DX11_DEVICE->CreateBlendState(&aDesc, &state);
	}
	DX11_CONTEXT->OMSetBlendState(state.Get(), aBlendFactor, aSampleMask);
}

void StateManager::GetBlendState(D3D11_BLEND_DESC& aDesc, FLOAT aBlendFactor[4], UINT& aSampleMask) const
{
	ComPtr<ID3D11BlendState> state{};
	DX11_CONTEXT->OMGetBlendState(&state, aBlendFactor, &aSampleMask);
	if (state)
		state->GetDesc(&aDesc);
}


//void ImGui::RasterizerDesc(D3D11_RASTERIZER_DESC& aDesc)
//{
//	{
//		int fillMode = aDesc.FillMode - D3D11_FILL_WIREFRAME;
//		Combo("Fill Mode", &fillMode, "Wireframe\0Solid\0\0");
//		aDesc.FillMode = static_cast<D3D11_FILL_MODE>(fillMode + D3D11_FILL_WIREFRAME);
//	}
//
//	{
//		int cullMode = aDesc.CullMode - D3D11_CULL_NONE;
//		Combo("Cull Mode", &cullMode, "None\0Front\0Back\0\0");
//		aDesc.CullMode = static_cast<D3D11_CULL_MODE>(cullMode + D3D11_CULL_NONE);
//	}
//
//	ImGui::Checkbox("Front Counter Clockwise", (bool*) & aDesc.FrontCounterClockwise);
//
//	ImGui::InputInt("Depth Bias", &aDesc.DepthBias);
//	ImGui::InputFloat("Depth Bias Clamp", &aDesc.DepthBiasClamp);
//	ImGui::InputFloat("Slope Scaled Depth Bias", &aDesc.SlopeScaledDepthBias);
//
//	ImGui::Checkbox("Depth Clip Enable", (bool*)&aDesc.DepthClipEnable);
//	ImGui::Checkbox("Scissor Enable", (bool*)&aDesc.ScissorEnable);
//	ImGui::Checkbox("Multisample Enable", (bool*)&aDesc.MultisampleEnable);
//	ImGui::Checkbox("Antialiased Line Enable", (bool*)&aDesc.AntialiasedLineEnable);
//}


