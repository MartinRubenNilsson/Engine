#include "pch.h"
#include "DearImGui.h"

bool DearImGui::Create(HWND hWnd, ID3D11Device* aDevice, ID3D11DeviceContext* aContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    if (!ImGui_ImplWin32_Init(hWnd))
        return false;
    if (!ImGui_ImplDX11_Init(aDevice, aContext))
        return false;

    struct Destroyer
    {
        ~Destroyer()
        {
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }
    };

    static Destroyer destroyer{};

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGuizmo::AllowAxisFlip(false);

    static constexpr ImWchar glyphRanges[]{ ICON_MIN_FA, ICON_MAX_16_FA, 0 };

    ImFontConfig config{};
    config.PixelSnapH = true;
    config.GlyphOffset.y = 1.f;
    config.GlyphRanges = glyphRanges;
    config.MergeMode = true;

    ScopedCurrentPath scopedPath{ fs::current_path() / "assets/fonts" };

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 13.f, &config);
    io.Fonts->Build();

    return true;
}

void DearImGui::NewFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuizmo::SetRect(0.f, 0.f, viewport->Size.x, viewport->Size.y);
    ImGui::DockSpaceOverViewport(viewport, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode);
}

void DearImGui::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
