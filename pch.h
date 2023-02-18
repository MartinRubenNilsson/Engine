#pragma once

// cstd
#include <cassert>
#include <cmath>
#include <cstdlib>

// std
#include <algorithm>
#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <initializer_list>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <variant>
#include <vector>
namespace fs = std::filesystem;

// win
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// dx11
#include <dxgi.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <directxtk/simplemath/SimpleMath.h>
using namespace DirectX;
using namespace DirectX::SimpleMath;

using SwapChainPtr = ComPtr<IDXGISwapChain>;
using BufferPtr = ComPtr<ID3D11Buffer>;
using TexturePtr = ComPtr<ID3D11Texture2D>;
using RenderTargetPtr = ComPtr<ID3D11RenderTargetView>;
using DepthStencilPtr = ComPtr<ID3D11DepthStencilView>;
using ShaderResourcePtr = ComPtr<ID3D11ShaderResourceView>;
using RasterizerStatePtr = ComPtr<ID3D11RasterizerState>;
using SamplerStatePtr = ComPtr<ID3D11SamplerState>;
using DepthStencilStatePtr = ComPtr<ID3D11DepthStencilState>;
using BlendStatePtr = ComPtr<ID3D11BlendState>;

// imgui
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

// entt
#include <entt/entt.hpp>

// other
#include "Singleton.h"
#include "Debug.h"
#include "DX11.h"
#include "ConstantBufferManager.h"
#include "Vertices.h"
#include "Shader.h"
#include "Scopes.h"