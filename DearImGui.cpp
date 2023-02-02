#include "pch.h"
#include "DearImGui.h"

DearImGui::DearImGui(HWND hWnd, ID3D11Device* aDevice, ID3D11DeviceContext* aContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    mySucceeded = ImGui_ImplWin32_Init(hWnd) && ImGui_ImplDX11_Init(aDevice, aContext);
}

DearImGui::~DearImGui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void DearImGui::NewFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void DearImGui::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
