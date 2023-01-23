#include "pch.h"
#include "DearImGui.h"

DearImGui::DearImGui(const Window& aWindow, const DX11& aDX11)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(aWindow.GetHandle());
    ImGui_ImplDX11_Init(aDX11.GetDevice(), aDX11.GetContext());
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
}

void DearImGui::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
