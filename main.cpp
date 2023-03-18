#include "pch.h"

// Windows
#include "Window.h"
#include "Drop.h"

// Rendering
#include "StateFactory.h"
#include "InputLayoutFactory.h"
#include "BackBuffer.h"
#include "Scopes.h"
#include "Renderer.h"

// Components
#include "EnttSerialization.h"
#include "Texture.h"
#include "Cubemap.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"

// Editor
#include "DearImGui.h"
#include "SceneViewManipulate.h"
#include "Manipulator.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Picker.h"
#include "PlayControls.h"

namespace
{
    bool theResize{ false };
    Drop theDrop{};

    fs::path GetModulePath()
    {
        WCHAR path[MAX_PATH];
        GetModuleFileName(GetModuleHandle(NULL), path, MAX_PATH);
        return { path };
    }
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    fs::current_path(GetModulePath().remove_filename());

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

    entt::registry registry{};
    json archive{};

    Camera editorCamera{};
    Matrix editorTransform{};

    PlayState state{};

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

        // Handle resized window
        if (theResize)
        {
            backBuffer.Resize();
            unsigned w = backBuffer.GetWidth();
            unsigned h = backBuffer.GetHeight();
            if (!renderer.ResizeTextures(w, h))
                return EXIT_FAILURE;
            editorCamera.SetAspect(static_cast<float>(w) / h);

            theResize = false;
        }

        // Handle dropped files
        if (theDrop)
        {
            for (fs::path& path : theDrop.GetPaths())
            {
                const fs::path extension{ path.extension() };

                if (extension == ".fbx")
                {
                    if (auto scene = sceneFactory.GetAsset(path))
                    {
                        archive = scene->GetRegistry();
                        registry = archive;
                    }
                }
                else if (extension == ".json")
                {
                    std::ifstream file{ path };
                    if (!file)
                        continue;

                    archive = json::parse(file, nullptr, false);
                    if (archive.is_discarded())
                        continue;

                    archive.get_to(registry);
                }
                else
                {
                    Debug::Println(std::format(
                        "Warning: Unsupported drop file extension {}: {}",
                        extension.string(), path.string()
                    ));
                }
            }

            theDrop = {};
        }

        imGui.NewFrame();

        ImGui::Begin(ICON_FA_EYE" Renderer");
        ImGui::Inspect(renderer);
        ImGui::End();

        ImGui::Begin(ICON_FA_LIST" Hierarchy");
        ImGui::Hierarchy(registry);
        ImGui::End();

        ImGui::Begin(ICON_FA_CIRCLE_INFO" Inspector");
        ImGui::Inspector(registry);
        ImGui::End();

        ImGui::SetNextWindowSize({});
        ImGui::Begin("Play Controls", NULL, ImGuiWindowFlags_NoDecoration);
        ImGui::PlayControls(state);
        ImGui::End();

        switch (state)
        {
        case PlayState::Stopped:
        {
            ImGui::Picker(registry);
            ImGui::SceneViewManipulate(editorTransform);
            ImGui::Manipulator(registry, editorCamera, editorTransform);
            renderer.SetCamera(editorCamera, editorTransform);
            break;
        }
        case PlayState::Starting:
        {
            archive = registry;
            state = PlayState::Started;
            break;
        }
        case PlayState::Started:
        {
            entt::entity entity = SortCamerasByDepth(registry);
            if (registry.all_of<Camera, Transform>(entity))
            {
                renderer.SetCamera(
                    registry.get<Camera>(entity),
                    registry.get<Transform>(entity).GetWorldMatrix(registry)
                );
            }
            break;
        }
        case PlayState::Stopping:
        {
            registry = archive;
            state = PlayState::Stopped;
            break;
        }
        }

        mouse.EndOfInputFrame();
        mouse.ResetScrollWheelValue();

        // Rendering
        {
            ScopedTargets scopedTarget{ backBuffer.GetTarget() };
            ScopedViewports scopedViewport{ backBuffer.GetViewport() };

            backBuffer.Clear();
            renderer.Render(registry);
            imGui.Render();

            backBuffer.Present();
        }
    }

	return static_cast<int>(msg.wParam);
}

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

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
