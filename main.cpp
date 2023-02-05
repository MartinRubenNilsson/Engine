#include "pch.h"
#include "Window.h"
#include "DearImGui.h"
#include "SwapChain.h"
#include "DepthBuffer.h"
#include "RasterizerStateManager.h"
#include "Scene.h"
#include "GeometryBuffer.h"
#include "FullscreenPass.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    fs::current_path(fs::current_path() / "Bin");

    DX11 dx11{};
    if (!dx11)
        return EXIT_FAILURE;

    ConstantBufferManager constantBufferMgr{};
    if (!constantBufferMgr)
        return EXIT_FAILURE;

    InputLayoutManager inputLayoutMgr{};
    ShaderManager shaderMgr{};
    RasterizerStateManager rasterizerStateMgr{};

    if (!inputLayoutMgr.CreateInputLayout<BasicVertex>())
        return EXIT_FAILURE;

    WindowClass windowClass{ WndProc };
	Window window{ windowClass };
	if (!window)
		return EXIT_FAILURE;

    DearImGui imGui{ window.GetHandle(), dx11.GetDevice(), dx11.GetContext() };
    if (!imGui)
        return EXIT_FAILURE;

    SwapChain swapChain{ window.GetHandle() };
    if (!swapChain)
        return EXIT_FAILURE;

    unsigned width, height;
    swapChain.GetDimensions(width, height);

    GeometryBuffer geometryBuffer{ width, height };
    if (!geometryBuffer)
        return EXIT_FAILURE;

    DepthBuffer depthBuffer{ width, height };
    if (!depthBuffer)
        return EXIT_FAILURE;

    SceneManager sceneMgr{};

    auto scene = sceneMgr.GetScene("mesh/test_00.fbx");
    if (!scene)
        return EXIT_FAILURE;

    window.SetTitle(L"Model Viewer");

    dx11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dx11.GetContext()->RSSetViewports(1, Viewport{ window.GetClientRect() }.Get11());

    auto vsBasic = shaderMgr.GetShader<VertexShader>("VsBasic.cso");
    auto psGBuffer = shaderMgr.GetShader<PixelShader>("PsGBuffer.cso");
    if (!vsBasic || !psGBuffer)
        return EXIT_FAILURE;

    FullscreenPass worldPosition{ shaderMgr.GetShader<PixelShader>("PsGBufferWorldPosition.cso") };
    if (!worldPosition)
        return EXIT_FAILURE;

    bool run = true;
    MSG msg{};

    while (run)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                run = false;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        auto& cameras = scene->GetCameras();
        auto& meshes = scene->GetMeshes();

        if (!cameras.empty())
        {
            auto& [camera, transform] = cameras.front();
            camera.SetCamera(transform->GetWorldMatrix());
        }

        vsBasic->SetShader();
        psGBuffer->SetShader();

        swapChain.ClearRenderTarget();
        depthBuffer.ClearDepth();
        geometryBuffer.SetRenderTargets(depthBuffer.GetDepthView());

        for (auto& [mesh, transforms] : meshes)
        {
            for (auto& transform : transforms)
                mesh.Draw(transform->GetWorldMatrix());
        }

        swapChain.SetRenderTarget();
        worldPosition.Draw();

        // ImGui
        {
            imGui.NewFrame();

            scene->ImGui();

            static bool showRasterizer = false;

            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("DirectX"))
                {
                    ImGui::MenuItem("Rasterizer", NULL, &showRasterizer);
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            if (showRasterizer)
            {
                if (ImGui::Begin("Rasterizer", &showRasterizer))
                    ImGui::RasterizerStateManager(rasterizerStateMgr);
                ImGui::End();
            }

            imGui.Render();
        }

        swapChain.Present();
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
    case WM_DESTROY:
        PostQuitMessage(EXIT_SUCCESS);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}