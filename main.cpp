#include "pch.h"
#include "Window.h"
#include "DearImGui.h"
#include "SwapChain.h"
#include "DepthBuffer.h"
#include "StateManager.h"
#include "InputLayoutManager.h"
#include "Scene.h"
#include "GeometryBuffer.h"
#include "FullscreenPass.h"
#include "Scopes.h"
#include "Image.h"
#include "Cubemap.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    // Set current path
    {
        char moduleFileName[MAX_PATH];
        GetModuleFileNameA(GetModuleHandleA(NULL), moduleFileName, MAX_PATH);
        fs::path modulePath{ moduleFileName };
        fs::current_path(modulePath.remove_filename());
    }

    DX11 dx11{};
    if (!dx11)
        return EXIT_FAILURE;

    ConstantBufferManager constantBufferMgr{};
    if (!constantBufferMgr)
        return EXIT_FAILURE;

    InputLayoutManager inputLayoutMgr{};
    if (!inputLayoutMgr)
        return EXIT_FAILURE;

    StateManager stateMgr{};
    ShaderManager shaderMgr{};

    WindowClass windowClass{ WndProc };
	Window window{ windowClass };
	if (!window)
		return EXIT_FAILURE;

    window.SetTitle(L"Model Viewer");

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

    DearImGui imGui{ window.GetHandle(), dx11.GetDevice(), dx11.GetContext() };
    if (!imGui)
        return EXIT_FAILURE;

    SceneManager sceneMgr{};

    auto scene = sceneMgr.GetScene("mesh/test_00.fbx");
    if (!scene)
        return EXIT_FAILURE;

    FullscreenPass fullscreenPasses[] =
    {
        PIXEL_SHADER("PsLightAmbient.cso"),
        PIXEL_SHADER("PsGBufferWorldPosition.cso"),
        PIXEL_SHADER("PsGBufferWorldNormal.cso"),
    };

    if (!std::ranges::all_of(fullscreenPasses, &FullscreenPass::operator bool))
        return EXIT_FAILURE;

    D3D11_SAMPLER_DESC samplerDesc{ CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} } };

    Viewport viewport{};
    viewport.width = static_cast<float>(width);
    viewport.height = static_cast<float>(height);
    std::vector<D3D11_VIEWPORT> viewports{ 8, viewport };

    ScopedPrimitiveTopology topologyScope{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
    ScopedSamplerStates samplersScope{ 0, { &samplerDesc, 1 } };
    ScopedViewports viewportsScope{ viewports };

    Image cubemapFaces[]
    {
        { "cubemap/Sorsele/posx.jpg", 4 },
        { "cubemap/Sorsele/negx.jpg", 4 },
        { "cubemap/Sorsele/posy.jpg", 4 },
        { "cubemap/Sorsele/negy.jpg", 4 },
        { "cubemap/Sorsele/posz.jpg", 4 },
        { "cubemap/Sorsele/negz.jpg", 4 },
    };

    Cubemap cubemap{ cubemapFaces };
    if (!cubemap)
        return EXIT_FAILURE;

    //Camera camera{}

    static int pass{};

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

        // Rendering
        swapChain.ClearRenderTargets();
        geometryBuffer.ClearRenderTargets();
        depthBuffer.ClearDepthStencil();
        ScopedRenderTargets swapChainScope{ swapChain };
        {
            auto& cameras = scene->GetCameras();
            if (!cameras.empty())
            {
                auto& [camera, transform] = cameras.front();
                camera.SetCamera(transform->GetWorldMatrix());
            }
        }
        {
            inputLayoutMgr.SetInputLayout(typeid(BasicVertex));
            ScopedShader vs{ VERTEX_SHADER("VsBasic.cso") };
            ScopedShader ps{ PIXEL_SHADER("PsGBuffer.cso") };
            ScopedRenderTargets geometryBufferScope{ geometryBuffer, depthBuffer };

            for (auto& [mesh, transforms] : scene->GetMeshes())
            {
                for (auto& transform : transforms)
                    mesh.Draw(transform->GetWorldMatrix());
            }
        }
        {
            ScopedPixelShaderResources resources{ 0, geometryBuffer };
            fullscreenPasses[pass].Draw();
        }
        {
            ScopedRenderTargets skyboxScope{ swapChain, depthBuffer };
            cubemap.DrawSkybox();
        }

        // ImGui
        {
            imGui.NewFrame();
            if (ImGui::Begin("Buffer"))
                ImGui::SliderInt("Buffer", &pass, 0, static_cast<int>(std::size(fullscreenPasses)) - 1);
            ImGui::End();
            scene->ImGui();
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