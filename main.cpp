#include "pch.h"
#include "Window.h"
#include "Drop.h"
#include "BackBuffer.h"
#include "DearImGui.h"
#include "StateManager.h"
#include "InputLayoutManager.h"
#include "Scene.h"
#include "Scopes.h"
#include "Camera.h"
#include "imgui_entt.h"
#include "Renderer.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
    Drop theDrop;
}

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    // Set current path
    {
        WCHAR moduleFileName[MAX_PATH];
        GetModuleFileName(GetModuleHandle(NULL), moduleFileName, MAX_PATH);
        fs::path modulePath{ moduleFileName };
        fs::current_path(modulePath.remove_filename());
    }

    WindowClass windowClass{ WndProc };
    Window window{ windowClass };
    if (!window)
        return EXIT_FAILURE;

    window.SetTitle(L"Model Viewer");

    DX11 dx11{};
    if (!dx11)
        return EXIT_FAILURE;

    DearImGui imGui{ window, dx11.GetDevice(), dx11.GetContext() };
    if (!imGui)
        return EXIT_FAILURE;

    InputLayoutManager inputLayoutMgr{};
    if (!inputLayoutMgr)
        return EXIT_FAILURE;

    ConstantBufferManager constantBufferMgr{};
    if (!constantBufferMgr)
        return EXIT_FAILURE;

    StateManager stateMgr{};
    ShaderManager shaderMgr{};

    BackBuffer backBuffer{ window };
    if (!backBuffer)
        return EXIT_FAILURE;

    Renderer renderer{ backBuffer.GetWidth(), backBuffer.GetHeight() };
    if (!renderer)
        return EXIT_FAILURE;

    SceneManager sceneMgr{};

    Matrix cameraTransform;
    const float cameraDistance = 20.f;
    cameraTransform.Translation({ 0.f, 0.f, -cameraDistance });

    entt::registry registry{};
    entt::entity selection{ entt::null };

    bool run = true;
    MSG msg{};

    while (run)
    {
        // Message loop
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                run = false;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Drag and drop
        if (theDrop)
        {
            registry.clear();
            sceneMgr.GetScene(theDrop.GetPaths().front())->Instantiate(registry);
            theDrop = {};
        }

        // Create camera
        PerspectiveCamera perspective{};
        perspective.fovY = 1.04719755119f; // 60 degrees
        perspective.aspect = backBuffer.GetViewport().AspectRatio();
        Camera camera{ perspective };

        // ImGui
        {
            imGui.NewFrame();

            ImGui::Begin("Buffer");
            ImGui::SliderInt("Buffer", &renderer.pass, 0, 6);
            ImGui::End();

            ImGui::ViewManipulate(camera, cameraTransform, cameraDistance, {}, { 150.f, 150.f }, 0);

            ImGui::Begin("Hierarchy");
            ImGui::Hierarchy(registry, selection);
            ImGui::End();

            ImGui::Begin("Inspector");
            ImGui::Inspector({ registry, selection });
            ImGui::End();

            if (auto transform = registry.try_get<Transform::Ptr>(selection))
            {
                auto operation = ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE;
                auto mode = ImGuizmo::LOCAL;

                Matrix m = (*transform)->GetWorldMatrix();
                ImGui::Manipulate(camera, cameraTransform, operation, mode, m);
                (*transform)->SetWorldMatrix(m);
            }

            camera.SetCamera(cameraTransform);
        }

        // Rendering
        {
            ScopedPrimitiveTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
            ScopedViewports scopedViewport{ backBuffer.GetViewport() };
            ScopedRenderTargets scopedTarget{ backBuffer };

            backBuffer.Clear();

            renderer.Render(registry);
            imGui.Render();

            backBuffer.Present();
        }
    }

	return static_cast<int>(msg.wParam);
}

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return 0;

    switch (msg)
    {
    case WM_DROPFILES:
        theDrop = { (HDROP)wParam };
        break;
    case WM_DESTROY:
        PostQuitMessage(EXIT_SUCCESS);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}
