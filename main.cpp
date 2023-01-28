#include "pch.h"
#include "Window.h"
#include "DearImGui.h"
#include "SwapChain.h"
#include "ConstantBuffer.h"

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

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    WindowClass windowClass{ WndProc };
	Window window{ windowClass };
	if (!window)
		return EXIT_FAILURE;

    DX11 dx11{};
    if (!dx11)
        return EXIT_FAILURE;
    DX11::Instance = &dx11;

    DearImGui imGui{ window.GetHandle(), dx11.GetDevice(), dx11.GetContext() };
    if (!imGui)
        return EXIT_FAILURE;

    SwapChain swapChain{ window };
    if (!swapChain)
        return EXIT_FAILURE;

    /*ConstantBuffer constantBuffer{ 16 };
    if (!constantBuffer)
        return EXIT_FAILURE;*/

    window.SetTitle(L"My Game");
    swapChain.SetRenderTarget();

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

        constexpr float clearColor[4] = { 0.6f, 0.5f, 0.5f, 1.f };
        swapChain.ClearRenderTarget(clearColor);
        imGui.NewFrame();

        /*
        * BEGIN RENDERING
        */


        /*
        * END RENDERING
        */

        imGui.Render();
        swapChain.Present();
    }

	return msg.wParam;
}