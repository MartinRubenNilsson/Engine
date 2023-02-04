#include "pch.h"
#include "Window.h"
#include "DearImGui.h"
#include "SwapChain.h"
#include "Shader.h"
#include "DepthBuffer.h"
#include "Scene.h"
#include "InputLayoutManager.h"
#include "RasterizerStateManager.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    fs::current_path(fs::current_path() / "Bin");

    WindowClass windowClass{ WndProc };
	Window window{ windowClass };
	if (!window)
		return EXIT_FAILURE;

    DX11 dx11{};
    if (!dx11)
        return EXIT_FAILURE;

    DearImGui imGui{ window.GetHandle(), dx11.GetDevice(), dx11.GetContext() };
    if (!imGui)
        return EXIT_FAILURE;

    ConstantBufferManager constantBufferMgr{};
    if (!constantBufferMgr)
        return EXIT_FAILURE;

    InputLayoutManager inputLayoutMgr{};
    if (!inputLayoutMgr)
        return EXIT_FAILURE;

    RasterizerStateManager rasterizerStateMgr{}; // todo

    SwapChain swapChain{ window.GetHandle() };
    if (!swapChain)
        return EXIT_FAILURE;

    DepthBuffer depthBuffer{ swapChain.GetWidth(), swapChain.GetHeight() };
    if (!depthBuffer)
        return EXIT_FAILURE;

    VertexShader vertexShader{ "VsBasic.cso" };
    PixelShader pixelShader{ "PsBasic.cso" };
    if (!vertexShader || !pixelShader)
        return EXIT_FAILURE;

    Scene scene{ "mesh/test_00.fbx" };
    if (!scene)
        return EXIT_FAILURE;

    window.SetTitle(L"Model Viewer");

    dx11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    vertexShader.SetShader();
    pixelShader.SetShader();
    swapChain.SetRenderTarget(depthBuffer.GetDepthStencil());
    dx11.GetContext()->RSSetViewports(1, Viewport{ window.GetClientRect() }.Get11());

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

        constexpr float clearColor[4] = { 0.f, 0.f, 0.f, 0.f };
        swapChain.ClearRenderTarget(clearColor);
        depthBuffer.ClearDepthStencil();
        imGui.NewFrame();

        ImGui::RasterizerStateManager(rasterizerStateMgr);

        scene.ImGui();
        scene.Render();

        imGui.Render();
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