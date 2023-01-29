#include "pch.h"
#include "Window.h"
#include "DearImGui.h"
#include "SwapChain.h"
#include "Shader.h"
#include "InputLayout.h"
#include "ConstantBuffer.h"
#include "Mesh.h"
#include "DepthBuffer.h"

#include "imgui_simplemath.h"

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

    SwapChain swapChain{ window.GetHandle() };
    if (!swapChain)
        return EXIT_FAILURE;

    DepthBuffer depthBuffer{ swapChain.GetWidth(), swapChain.GetHeight() };
    if (!depthBuffer)
        return EXIT_FAILURE;

    const fs::path currentPath = fs::current_path() / "Bin";
    fs::current_path(currentPath);

    Mesh mesh("mesh/bunny.obj");
    if (!mesh)
        return EXIT_FAILURE;

    VertexShader vertexShader(currentPath / "VsBasic.cso");
    PixelShader pixelShader(currentPath / "PsBasic.cso");
    if (!vertexShader || !pixelShader)
        return EXIT_FAILURE;

    InputLayout inputLayout{ vertexShader.GetBytecode() };
    if (!inputLayout)
        return EXIT_FAILURE;

    struct CameraData
    {
        Matrix worldToClipMatrix;
        Vector4 cameraPosition;
    };

    ConstantBuffer cameraBuffer{ sizeof(CameraData) };
    ConstantBuffer modelBuffer{ sizeof(Matrix) };
    if (!cameraBuffer || !modelBuffer)
        return EXIT_FAILURE;

    Viewport viewport{ window.GetClientRect() };

    dx11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cameraBuffer.VSSetConstantBuffer(0);
    modelBuffer.VSSetConstantBuffer(1);
    inputLayout.SetInputLayout();
    vertexShader.SetShader();
    pixelShader.SetShader();
    swapChain.SetRenderTarget(depthBuffer.GetDepthStencil());
    dx11.GetContext()->RSSetViewports(1, viewport.Get11());

    Matrix cameraTransform{};
    Matrix cameraProjection{ XMMatrixPerspectiveFovLH(90.f, viewport.AspectRatio(), 0.01f, 100.f) };
    Matrix modelTransform{};
    modelTransform.Translation({ 0.f, -10.f, 20.f });
    modelTransform = Matrix::CreateScale(100.f) * modelTransform;

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

        {
            CameraData cameraData{};
            cameraData.worldToClipMatrix = cameraTransform.Invert() * cameraProjection;
            Vector3 position = cameraTransform.Translation();
            cameraData.cameraPosition = { position.x, position.y, position.z, 1.f };
            cameraBuffer.UpdateConstantBuffer(&cameraData);
        }

        modelBuffer.UpdateConstantBuffer(&modelTransform);

        mesh.Draw();

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