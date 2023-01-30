#include "pch.h"
#include "Window.h"
#include "DearImGui.h"
#include "SwapChain.h"
#include "Shader.h"
#include "InputLayout.h"
#include "Mesh.h"
#include "DepthBuffer.h"
#include "Camera.h"

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

    std::vector<Mesh> meshes{};
    std::vector<aiMaterial*> materials{};

    {
        Assimp::Importer importer{};
        const unsigned flags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality;

        if (auto scene = importer.ReadFile("mesh/bunny.obj", flags))
        {
            for (auto mesh : std::span(scene->mMeshes, scene->mNumMeshes))
                meshes.emplace_back(*mesh);

            for (auto material : std::span(scene->mMaterials, scene->mNumMaterials))
                materials.emplace_back(material);
        }
    }

    VertexShader vertexShader(currentPath / "VsBasic.cso");
    PixelShader pixelShader(currentPath / "PsBasic.cso");
    if (!vertexShader || !pixelShader)
        return EXIT_FAILURE;

    InputLayout inputLayout{ vertexShader.GetBytecode() };
    if (!inputLayout)
        return EXIT_FAILURE;

    Viewport viewport{ window.GetClientRect() };

    dx11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    inputLayout.SetInputLayout();
    vertexShader.SetShader();
    pixelShader.SetShader();
    swapChain.SetRenderTarget(depthBuffer.GetDepthStencil());
    dx11.GetContext()->RSSetViewports(1, viewport.Get11());

    Matrix cameraTransform{};
    Matrix meshTransform{};
    meshTransform.Translation({ 0.f, -10.f, 20.f });
    meshTransform = Matrix::CreateScale(100.f) * meshTransform;

    Camera camera{};

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
                ImGui::DragTransform("Model", &meshTransform);
            ImGui::End();
        }

        camera.UseForDrawing(cameraTransform);

        if (!meshes.empty())
            meshes.front().Draw(meshTransform);

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