#include "pch.h"
#include "DearImGui.h"

DearImGui::DearImGui(HWND hWnd, ID3D11Device* aDevice, ID3D11DeviceContext* aContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (!ImGui_ImplWin32_Init(hWnd))
        return;
    if (!ImGui_ImplDX11_Init(aDevice, aContext))
        return;

    AddFonts();

    mySucceeded = true;
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

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0.f, 0.f, io.DisplaySize.x, io.DisplaySize.y);
}

void DearImGui::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DearImGui::AddFonts()
{
    static constexpr ImWchar glyphRanges[]{ ICON_MIN_FA, ICON_MAX_16_FA, 0 };

    ImFontConfig config{};
    config.GlyphOffset.y = 2.f;
    config.GlyphRanges = glyphRanges;
    config.MergeMode = true;

    constexpr float size = 14.f;

    const fs::path currentPath{ fs::current_path() };
    fs::current_path(currentPath / "font");

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAR, size, &config);
    io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, size, &config);
    io.Fonts->Build();

    fs::current_path(currentPath);
}
