#include "pch.h"

// Windows
#include "Window.h"
#include "Drop.h"

// Physics
#include "PhysX.h"
#include "PhysicMaterial.h"

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
#include "Systems.h"
#include "Console.h"

namespace
{
    bool theResize{ false };
}

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    fs::current_path(GetExePath().remove_filename());

    Keyboard keyboard{};
    Mouse mouse{};

    if (!PhysX::Create())
        return EXIT_FAILURE;

    if (!DX11::Create())
        return EXIT_FAILURE;

    if (!Window::Create())
        return EXIT_FAILURE;

    Window::SetIcon("icon.ico");
    mouse.SetWindow(Window::GetHandle());

    if (!DearImGui::Create(Window::GetHandle(), DX11::GetDevice(), DX11::GetContext()))
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

    BackBuffer backBuffer{ Window::GetHandle() };
    if (!backBuffer)
        return EXIT_FAILURE;

    Renderer renderer{ backBuffer.GetWidth(), backBuffer.GetHeight() };
    if (!renderer)
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
    float simulationAccumulator = 0.f;

    PlayState state{};
    MSG msg{};

    while (state != PlayState::Quit)
    {
        Window::SetTitle(archivePath.stem().string());

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
            unsigned w = backBuffer.GetWidth();
            unsigned h = backBuffer.GetHeight();
            if (!renderer.ResizeTextures(w, h))
                return EXIT_FAILURE;
            editorCamera.SetAspect(static_cast<float>(w) / h);
            theResize = false;
        }

        // Handle dropped files
        if (Drop::Begin())
        {
            for (fs::path& path : Drop::GetPaths())
            {
                const fs::path extension{ path.extension() };

                if (extension == ".fbx")
                {
                    if (auto scene = sceneFactory.GetAsset(path))
                    {
                        entt::entity rootEntity = scene->Instantiate(registry);
                        Select(registry, rootEntity);
                    }
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
            Drop::End();
        }

        DearImGui::NewFrame();

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
            ImGui::Begin("PhysX");
            ImGui::PhysX();
            ImGui::End();

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
            deltaTimer = {};
            simulationAccumulator = 0.f;
            state = PlayState::Started;
            break;
        }
        case PlayState::Started:
        {
            const float dt = deltaTimer.Query();
            registry.ctx().insert_or_assign("deltaTime"_hs, dt);

            Systems::Update(registry);

            static constexpr float simulationStepSize = 1.f / 60.f;
            simulationAccumulator += dt;

            if (simulationAccumulator >= simulationStepSize)
            {
                simulationAccumulator -= simulationStepSize;
                PhysX::GetScene()->simulate(simulationStepSize);
                PhysX::GetScene()->fetchResults(true);
            }

            SortCamerasByDepth(registry); // todo: make into system
            GetFirstCamera(registry, camera, cameraTransform); // todo: make into system

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
            renderer.SetCamera(camera, cameraTransform);
            renderer.Render(registry);
            DearImGui::Render();
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
        Drop::Accept((HDROP)wParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
