#include "pch.h"
#include "Window.h"
#include "Drop.h"
#include "DearImGui.h"
#include "StateFactory.h"
#include "InputLayoutManager.h"
#include "BackBuffer.h"
#include "Renderer.h"
#include "Texture.h"
#include "Scene.h"
#include "Scopes.h"
#include "Camera.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "GameScene.h"
#include "Tags.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
    bool theResize{ false };
    Drop theDrop{};
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

    DearImGui imGui{ window, dx11.GetDevice(), dx11.GetContext() };
    if (!imGui)
        return EXIT_FAILURE;

    BackBuffer backBuffer{ window };
    if (!backBuffer)
        return EXIT_FAILURE;

    InputLayoutManager inputLayoutMgr{};
    if (!inputLayoutMgr)
        return EXIT_FAILURE;

    StateFactory stateFactory{};
    ShaderFactory shaderFactory{};

    TextureFactory textureFactory{};
    SceneFactory sceneFactory{};
    CubemapFactory cubemapFactory{};

    Renderer renderer{ backBuffer.GetWidth(), backBuffer.GetHeight() };
    if (!renderer)
        return EXIT_FAILURE;

    Camera camera{};
    Matrix cameraTransform{};

    entt::registry registry{};
    GameScene gameScene{ registry };

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
            backBuffer.Resize();
            renderer.ResizeTextures(backBuffer.GetWidth(), backBuffer.GetHeight());
            theResize = false;
        }

        // Drag and drop
        if (theDrop)
        {
            registry.clear();
            sceneFactory.GetAsset(theDrop.GetPaths().front())->CopyTo(registry);
            theDrop = {};
        }

        // Editor camera
        {
            PerspectiveCamera perspective{};
            perspective.fovY = 1.04719755119f; // 60 degrees
            perspective.aspect = backBuffer.GetViewport().AspectRatio();
            camera.SetVariant(perspective);

            Mouse::State state{ mouse.GetState() };
            mouse.SetMode(state.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

            if (state.positionMode == Mouse::MODE_RELATIVE)
            {
                const float rotSpeed = 0.25f;
                const float moveSpeed = ImGui::IsKeyDown(ImGuiKey_LeftShift) ? 15.f : 5.f;
                const float deltaTime = ImGui::GetIO().DeltaTime;

                if (state.x != 0 || state.y != 0)
                {
                    Vector3 scale{}, translation{};
                    Quaternion rotation{};
                    cameraTransform.Decompose(scale, rotation, translation);

                    rotation = Quaternion::CreateFromAxisAngle(Vector3::UnitX, state.y * rotSpeed * deltaTime) * rotation;
                    rotation = rotation * Quaternion::CreateFromAxisAngle(Vector3::UnitY, state.x * rotSpeed * deltaTime);
                
                    cameraTransform = Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(translation);
                }

                Vector3 moveDir{};

                if (ImGui::IsKeyDown(ImGuiKey_D)) moveDir += Vector3::UnitX;
                if (ImGui::IsKeyDown(ImGuiKey_A)) moveDir -= Vector3::UnitX;
                if (ImGui::IsKeyDown(ImGuiKey_E)) moveDir += Vector3::UnitY;
                if (ImGui::IsKeyDown(ImGuiKey_Q)) moveDir -= Vector3::UnitY;
                if (ImGui::IsKeyDown(ImGuiKey_W)) moveDir += Vector3::UnitZ;
                if (ImGui::IsKeyDown(ImGuiKey_S)) moveDir -= Vector3::UnitZ;
            
                if (moveDir != Vector3::Zero)
                    cameraTransform = Matrix::CreateTranslation(moveDir * moveSpeed * deltaTime) * cameraTransform;
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::GetIO().WantCaptureMouse)
            {
                auto selected = registry.view<Tag::Selected>();
                registry.erase<Tag::Selected>(selected.begin(), selected.end());

                auto x = static_cast<unsigned>(ImGui::GetMousePos().x);
                auto y = static_cast<unsigned>(ImGui::GetMousePos().y);

                entt::entity selection = renderer.PickEntity(x, y);

                if (registry.valid(selection))
                    registry.emplace_or_replace<Tag::Selected>(selection);
            }
        }

        mouse.EndOfInputFrame();

        // ImGui
        {
            imGui.NewFrame();

            ImGui::Begin(ICON_FA_LIST" Hierarchy");
            ImGui::Hierarchy(registry);
            ImGui::End();

            ImGui::Begin(ICON_FA_CIRCLE_INFO" Inspector");
            ImGui::Inspector(registry);
            ImGui::End();

            entt::entity selection{ registry.view<Tag::Selected>().front() };

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
            }

            ImGui::Begin(ICON_FA_CHART_SIMPLE" Render Statistics");
            ImGui::InspectRenderStatistics(renderer.GetStatistics());
            ImGui::End();

            renderer.SetCamera(camera, cameraTransform);
        }

        // Rendering
        {
            ScopedViewports scopedViewport{ backBuffer.GetViewport() };
            ScopedRenderTargets scopedTarget{ backBuffer.GetTarget() };

            backBuffer.Clear();
            renderer.RenderScene(registry);
            //renderer.RenderDebug(t_AmbientAccessMap);
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
