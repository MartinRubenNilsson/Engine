#include "pch.h"
#include "Window.h"
#include "DearImGui.h"
#include "SwapChain.h"
#include "Shader.h"
#include "InputLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

#include "imgui_simplemath.h"

#pragma comment(lib, "assimp-vc142-mt") 

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

    const fs::path currentPath = fs::current_path() / "Bin";
    fs::current_path(currentPath);

    VertexShader vertexShader(currentPath / "VsBasic.cso");
    PixelShader pixelShader(currentPath / "PsBasic.cso");
    if (!vertexShader || !pixelShader)
        return EXIT_FAILURE;

    BasicInputLayout inputLayout{ vertexShader.GetBytecode() };
    if (!inputLayout)
        return EXIT_FAILURE;

    struct Vertex
    {
        float position[4];
    };

    constexpr float extent = 10.f;

    const Vertex vertices[] =
    {
        { 0.f,  0.f,  0.f, 1.f },
        { 0.f,  extent, 0.f, 1.f },
        { extent, 0.f,  0.f, 1.f },
        { extent, extent, 0.f, 1.f }
    };

    const UINT indices[] =
    {
        0, 1, 2,
        2, 1, 3
    };

    VertexBuffer vertexBuffer{ std::span(vertices) };
    IndexBuffer indexBuffer{ indices };
    if (!vertexBuffer || !indexBuffer)
        return EXIT_FAILURE;

    ConstantBuffer cameraBuffer{ sizeof(Matrix) };
    ConstantBuffer modelBuffer{ sizeof(Matrix) };
    if (!cameraBuffer || !modelBuffer)
        return EXIT_FAILURE;

    Viewport viewport{ window.GetClientRect() };

    dx11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cameraBuffer.VSSetConstantBuffer(0);
    modelBuffer.VSSetConstantBuffer(1);
    inputLayout.SetInputLayout();
    vertexBuffer.SetVertexBuffer();
    indexBuffer.SetIndexBuffer();
    vertexShader.SetShader();
    pixelShader.SetShader();
    swapChain.SetRenderTarget();
    dx11.GetContext()->RSSetViewports(1, viewport.Get11());

    Matrix cameraTransform{};
    Matrix cameraProjection{ XMMatrixPerspectiveFovLH(90.f, viewport.AspectRatio(), 0.01f, 100.f) };
    Matrix modelTransform{};

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
        imGui.NewFrame();

        /*
        * BEGIN RENDERING
        */

        if (ImGui::Begin("Debug"))
        {
            if (ImGui::CollapsingHeader("Camera"))
                ImGui::DragTransform("Camera", &cameraTransform);
            if (ImGui::CollapsingHeader("Model"))
                ImGui::DragTransform("Model", &modelTransform);
            ImGui::End();
        }

        Matrix worldToClipMatrix = cameraTransform.Invert() * cameraProjection;
        cameraBuffer.UpdateConstantBuffer(&worldToClipMatrix);
        modelBuffer.UpdateConstantBuffer(&modelTransform);

        dx11.GetContext()->DrawIndexed((UINT)indexBuffer.GetIndexCount(), 0, 0);

        /*
        * END RENDERING
        */

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