#include "pch.h"

// Windows
#include "Window.h"
#include "Drop.h"

// Physics
#include "PhysX.h"

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
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"

// Editor
#include "DearImGui.h"
#include "SceneViewManipulate.h"
#include "Manipulator.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Picker.h"
#include "PlayControls.h"
#include "Menus.h"

// Time
#include "DeltaTimer.h"

// Other
#include "EngineAsset.h"
#include "Win32Common.h"

namespace
{
    bool theResize{ false };
    Drop theDrop{};
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    fs::current_path(GetExePath().remove_filename());

    Keyboard keyboard{};
    Mouse mouse{};

    Window window{ WndProc };
    if (!window)
        return EXIT_FAILURE;

    window.SetIcon("icon.ico");
    mouse.SetWindow(window);

    PhysX physX{};
    if (!physX)
        return EXIT_FAILURE;

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
    ScopedResources scopedResources
    {
        ShaderType::Pixel, t_GaussianMap,
        {
            CreateGaussianMap(),
            CreateIntegrationMap(),
            CreateUniformMap(),
        }
    };

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

    entt::registry registry{};
    json archive{};
    fs::path archivePath{};

    Camera editorCamera{};
    Matrix editorCameraTransform{};

    DeltaTimer deltaTimer{};
    float simulationTime = 0.f;
    bool simulate = false;

    PlayState state{};
    MSG msg{};

    while (state != PlayState::Quit)
    {
        window.SetTitle(archivePath.stem().wstring());

        // Message loop
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                state = PlayState::Quit;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Handle resized window
        if (theResize)
        {
            backBuffer.Resize();
            if (!renderer.ResizeTextures(backBuffer.GetWidth(), backBuffer.GetHeight()))
                return EXIT_FAILURE;
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
                        Select(registry, scene->Instantiate(registry));
                }
                else if (extension == ".png")
                {
                    textureFactory.GetAsset(path);
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
        {
            std::ranges::fill(ImGui::GetIO().MouseReleased, false);
            //ImGui::BeginDisabled();
        }

        {
            MenuCommand cmd = MenuCommand::None;

            ImGui::Shortcut(cmd);

            if (ImGui::BeginMainMenuBar())
            {
                ImGui::MainMenu(cmd);
                ImGui::EndMainMenuBar();
            }

            fs::path newArchivePath = archivePath;
            GetPath(cmd, newArchivePath);

            switch (cmd)
            {
            case MenuCommand::NewScene:
            {
                registry.clear();
                archive.clear();
                archivePath = newArchivePath;
                std::ofstream{ newArchivePath } << archive;
                break;
            }
            case MenuCommand::OpenScene:
            {
                json newArchive{ json::parse(std::ifstream{ newArchivePath }, nullptr, false) };
                if (newArchive.is_discarded())
                {
                    Debug::Println(std::format(
                        "Error: Failed to parse scene: {}",
                        newArchivePath.string()
                    ));
                }
                else
                {
                    registry = newArchive;
                    archive = newArchive;
                    archivePath = newArchivePath;
                }
                break;
            }
            case MenuCommand::Save:
                [[fallthrough]];
            case MenuCommand::SaveAs:
            {
                archive = registry;
                std::ofstream{ newArchivePath } << archive;
                break;
            }
            case MenuCommand::Exit:
                state = PlayState::Quit;
                break;
            }
        }

        {
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

            if (state == PlayState::Stopped)
            {
                ImGui::Picker(registry);
                ImGui::SceneViewManipulate(editorCameraTransform, keyboardState, mouseState);

                static ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
                static ImGuizmo::MODE mode = ImGuizmo::LOCAL;

                if (mouseState.positionMode != Mouse::MODE_RELATIVE)
                {
                    ImGui::Shortcut(op);
                    ImGui::Shortcut(mode);
                }

                ImGui::SetNextWindowSize({});
                ImGui::Begin("Manipulator", NULL, ImGuiWindowFlags_NoDecoration);
                ImGui::Manipulator(registry, editorCamera, editorCameraTransform, op, mode);
                ImGui::End();
            }
        }

        /*if (mouseState.positionMode == Mouse::MODE_RELATIVE)
            ImGui::EndDisabled();*/

        Camera camera{ editorCamera };
        Matrix cameraTransform{ editorCameraTransform };

        switch (state)
        {
        case PlayState::Stopped:
        {
            // todo: something here?
            break;
        }
        case PlayState::Starting:
        {
            archive = registry;
            physX.ConnectPvd();
            deltaTimer = {};
            simulationTime = 0.f;
            state = PlayState::Started;
            break;
        }
        case PlayState::Started:
        {
            if (simulate)
                physX.GetScene()->fetchResults(true);

            const float deltaTime = deltaTimer.Query();
            simulationTime += deltaTime;

            // todo: game logic here

            static constexpr float simulationStep = 1.f / 60.f;
            simulate = (simulationTime >= simulationStep);

            if (simulate)
            {
                simulationTime -= simulationStep;
                physX.GetScene()->simulate(simulationStep);
            }

            SortCamerasByDepth(registry);
            GetFirstCamera(registry, camera, cameraTransform);

            break;
        }
        case PlayState::Stopping:
        {
            physX.DisconnectPvd();
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
            renderer.SetCamera(camera, cameraTransform);
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
