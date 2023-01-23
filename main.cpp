#include "pch.h"
#include "SwapChain.h"
#include "DearImGui.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

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

    window.SetTitle(L"My Game");

    DX11 dx11{};
    if (!dx11)
        return EXIT_FAILURE;

    SwapChain swapChain{ window };
    if (!swapChain)
        return EXIT_FAILURE;

    DearImGui imGui{ window, dx11 };

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
        swapChain.SetRenderTarget();

        imGui.NewFrame();
        if (ImGui::Begin("Test"))
        {
            if (ImGui::Button("Push me!"))
                Debug::Println("You pushed me!");
            ImGui::End();
        }
        imGui.Render();

        swapChain.Present();
    }

	return msg.wParam;
}