#pragma once

// win
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// imgui
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui.h>

// dx11
#include <dxgi.h>
#include <d3d11.h>
#include <directxtk/simplemath/SimpleMath.h>
using namespace DirectX::SimpleMath;

// assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

// std
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
namespace fs = std::filesystem;

// cstd
#include <cassert>
#include <cmath>
#include <cstdlib>

// other
#include "Debug.h"
#include "DX11.h"
#include "ConstantBufferManager.h"