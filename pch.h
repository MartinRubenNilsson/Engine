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

// std
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

// cstd
#include <cassert>
#include <cstdlib>

// other
#include "Debug.h"
#include "DX11.h"