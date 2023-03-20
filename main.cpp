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
#include "EnttCommon.h"
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
#include "Menus.h"

// Other
#include "EngineAsset.h"

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

    Keyboard keyboard{};
    Mouse mouse{};

    Window window{ WndProc };
    if (!window)
        return EXIT_FAILURE;

    window.SetIcon("icon.ico");
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

    if (!ImportEngineAssets())
        return EXIT_FAILURE;

    entt::registry sceneReg{};
    json sceneJson{};
    fs::path scenePath{};
    Camera sceneCam{};
    Matrix sceneCamTrans{};

    PlayState state{};

    bool run = true;
    MSG msg{};

    while (run)
    {
        window.SetTitle(scenePath.stem().wstring());

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
            sceneCam.SetAspect(static_cast<float>(w) / h);

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
                        Select(sceneReg, scene->Instantiate(sceneReg));
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

        const Keyboard::State keyboardState{ keyboard.GetState() };
        const Mouse::State mouseState{ mouse.GetState() };

        mouse.SetMode(mouseState.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

        /*
        * When we release right mouse button and exit relative mode,
        * there is one frame where mouse might be over a ImGui window.
        * Then, a context menu might open. Therefore we clear mouse state.
        */
        if (mouseState.positionMode == Mouse::MODE_RELATIVE)
            std::ranges::fill(ImGui::GetIO().MouseReleased, false);

        {
            MenuCommand cmd = MenuCommand::None;

            ImGui::Shortcut(cmd);

            if (ImGui::BeginMainMenuBar())
            {
                ImGui::MainMenu(cmd);
                ImGui::EndMainMenuBar();
            }

            fs::path newScenePath = scenePath;
            GetPath(cmd, newScenePath);

            switch (cmd)
            {
            case MenuCommand::NewScene:
            {
                sceneReg.clear();
                sceneJson.clear();
                scenePath = newScenePath;
                std::ofstream{ newScenePath } << sceneJson;
                break;
            }
            case MenuCommand::OpenScene:
            {
                json newSceneJson{ json::parse(std::ifstream{ newScenePath }, nullptr, false) };
                if (newSceneJson.is_discarded())
                {
                    Debug::Println(std::format(
                        "Error: Failed to parse scene: {}",
                        newScenePath.string()
                    ));
                }
                else
                {
                    sceneReg = newSceneJson;
                    sceneJson = newSceneJson;
                    scenePath = newScenePath;
                }
                break;
            }
            case MenuCommand::Save:
                [[fallthrough]];
            case MenuCommand::SaveAs:
            {
                sceneJson = sceneReg;
                std::ofstream{ newScenePath } << sceneJson;
                break;
            }
            case MenuCommand::Exit:
                return EXIT_SUCCESS;
            }
        }

        {
            ImGui::Begin(ICON_FA_EYE" Renderer");
            ImGui::Inspect(renderer);
            ImGui::End();

            ImGui::Begin(ICON_FA_LIST" Hierarchy");
            ImGui::Hierarchy(sceneReg);
            ImGui::End();

            ImGui::Begin(ICON_FA_CIRCLE_INFO" Inspector");
            ImGui::Inspector(sceneReg);
            ImGui::End();

            ImGui::SetNextWindowSize({});
            ImGui::Begin("Play Controls", NULL, ImGuiWindowFlags_NoDecoration);
            ImGui::PlayControls(state);
            ImGui::End();
        }

        switch (state)
        {
        case PlayState::Stopped:
        {
            ImGui::Picker(sceneReg);
            ImGui::SceneViewManipulate(sceneCamTrans, keyboardState, mouseState);
            ImGui::Manipulator(sceneReg, sceneCam, sceneCamTrans);
            renderer.SetCamera(sceneCam, sceneCamTrans);
            break;
        }
        case PlayState::Starting:
        {
            sceneJson = sceneReg;
            state = PlayState::Started;
            break;
        }
        case PlayState::Started:
        {
            entt::entity entity = SortCamerasByDepth(sceneReg);
            if (sceneReg.all_of<Camera, Transform>(entity))
            {
                renderer.SetCamera(
                    sceneReg.get<Camera>(entity),
                    sceneReg.get<Transform>(entity).GetWorldMatrix(sceneReg)
                );
            }
            break;
        }
        case PlayState::Stopping:
        {
            sceneReg = sceneJson;
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
            renderer.Render(sceneReg);
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
