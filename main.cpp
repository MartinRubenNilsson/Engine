#include "pch.h"
#include "Window.h"
#include "Drop.h"
#include "DearImGui.h"
#include "StateFactory.h"
#include "InputLayoutFactory.h"
#include "BackBuffer.h"
#include "Renderer.h"
#include "Texture.h"
#include "Scene.h"
#include "Scopes.h"
#include "Camera.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Tags.h"
#include "Cubemap.h"
#include "JsonArchive.h"
#include "SceneViewManipulate.h"
#include "Picker.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
    bool theResize{ false };
    Drop theDrop{};
}

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    // Set current path to module path
    {
        WCHAR path[MAX_PATH];
        GetModuleFileName(GetModuleHandle(NULL), path, MAX_PATH);
        fs::current_path(fs::path{ path }.remove_filename());
    }

    Window window{ Window::Register(WndProc) };
    if (!window)
        return EXIT_FAILURE;

    window.SetTitle(L"Model Viewer");

    Keyboard keyboard{};
    Mouse mouse{};

    mouse.SetWindow(window);

    DX11 dx11{};
    if (!dx11)
        return EXIT_FAILURE;

    StateFactory stateFactory{};
    ShaderFactory shaderFactory{};
    InputLayoutFactory layoutFactory{};
    if (!layoutFactory)
        return EXIT_FAILURE;

    ScopedTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
    ScopedSamplers scopedSamplers{ 0, GetSamplerDescs() };
    ScopedResources scopedResources{ ShaderType::Pixel, t_GaussianMap, { CreateGaussianMap(), CreateIntegrationMap() } };

    BackBuffer backBuffer{ window };
    if (!backBuffer)
        return EXIT_FAILURE;

    Renderer renderer{ backBuffer.GetWidth(), backBuffer.GetHeight() };
    if (!renderer)
        return EXIT_FAILURE;

    DearImGui imGui{ window, dx11.GetDevice(), dx11.GetContext() };
    if (!imGui)
        return EXIT_FAILURE;

    TextureFactory textureFactory{};
    CubemapFactory cubemapFactory{};
    SceneFactory sceneFactory{};

    Scene::Ptr scene{};

    Camera camera{};
    Matrix cameraTransform{};

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

        // Resize
        if (theResize)
        {
            theResize = false;
            backBuffer.Resize();
            renderer.ResizeTextures(backBuffer.GetWidth(), backBuffer.GetHeight());
            camera.SetAspect((float)backBuffer.GetWidth() / (float)backBuffer.GetHeight());
        }

        // Drag and drop
        if (theDrop)
        {
            fs::path path{ theDrop.GetPaths().front() };
            theDrop = {};

            scene = sceneFactory.GetAsset(path);

            /*JsonArchive archive{};
            archive.Save(registry);

            std::ofstream file{ path.parent_path() / "registry.json" };
            if (file)
                file << std::setw(4) << archive.GetJson();*/
        }

        // ImGui & Editor
        {
            imGui.NewFrame();

            ImGui::SceneViewManipulate(cameraTransform);

            if (scene)
                ImGui::Picker(scene->GetRegistry());

            ImGui::Begin(ICON_FA_LIST" Hierarchy");
            if (scene)
                ImGui::Hierarchy(scene->GetRegistry());
            ImGui::End();

            ImGui::Begin(ICON_FA_CIRCLE_INFO" Inspector");
            //ImGui::Inspector(registry);
            ImGui::End();

            /*entt::entity selection{ registry.view<Tag::Selected>().front() };

            if (auto transform = registry.try_get<Transform::Ptr>(selection))
            {
                static ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
                static ImGuizmo::MODE mode = ImGuizmo::LOCAL;

                if (ImGui::IsKeyPressed(ImGuiKey_W))
                    op = ImGuizmo::TRANSLATE;
                if (ImGui::IsKeyPressed(ImGuiKey_E))
                    op = ImGuizmo::ROTATE;
                if (ImGui::IsKeyPressed(ImGuiKey_R))
                    op = ImGuizmo::SCALE;

                if (ImGui::IsKeyPressed(ImGuiKey_X))
                    mode = static_cast<ImGuizmo::MODE>(1 - mode);

                Matrix m = (*transform)->GetWorldMatrix();
                ImGui::Manipulate(camera, cameraTransform, op, mode, m);
                (*transform)->SetWorldMatrix(m);
            }*/

            ImGui::Begin(ICON_FA_EYE" Renderer");
            ImGui::InspectRenderer(renderer);
            ImGui::End();

            renderer.SetCamera(camera, cameraTransform);
        }

        mouse.EndOfInputFrame();
        mouse.ResetScrollWheelValue();

        // Rendering
        {
            ScopedViewports scopedViewport{ backBuffer.GetViewport() };
            ScopedTargets scopedTarget{ backBuffer.GetTarget() };

            backBuffer.Clear();
            if (scene)
                renderer.Render(scene->GetRegistry());
            imGui.Render();

            backBuffer.Present();
        }
    }

	return static_cast<int>(msg.wParam);
}

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return 0;

    if (ImGui::GetCurrentContext())
    {
        if (!ImGui::GetIO().WantCaptureKeyboard)
            Keyboard::ProcessMessage(message, wParam, lParam);
        if (!ImGui::GetIO().WantCaptureMouse)
            Mouse::ProcessMessage(message, wParam, lParam);
    }

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(EXIT_SUCCESS);
        break;
    case WM_SIZE:
        if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
            theResize = true;
        break;
    case WM_EXITSIZEMOVE:
        theResize = true;
        break;
    case WM_DROPFILES:
        theDrop = { (HDROP)wParam };
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
