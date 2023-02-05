#include "pch.h"
#include "Window.h"
#include "DearImGui.h"
#include "SwapChain.h"
#include "DepthBuffer.h"
#include "RasterizerStateManager.h"
#include "Scene.h"
#include "GeometryBuffer.h"
#include "FullscreenPass.h"
#include "Scopes.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    {
        char moduleFileName[MAX_PATH];
        GetModuleFileNameA(GetModuleHandleA(NULL), moduleFileName, MAX_PATH);

        fs::path modulePath{ moduleFileName };
        modulePath.remove_filename();

        fs::current_path(modulePath);
    }

    DX11 dx11{};
    if (!dx11)
        return EXIT_FAILURE;

    ConstantBufferManager constantBufferMgr{};
    if (!constantBufferMgr)
        return EXIT_FAILURE;

    ShaderManager shaderMgr{};

    InputLayoutManager inputLayoutMgr{};
    if (!inputLayoutMgr.RegisterInputLayout<BasicVertex>())
        return EXIT_FAILURE;
    if (!inputLayoutMgr.RegisterInputLayout<FullscreenVertex>())
        return EXIT_FAILURE;

    RasterizerStateManager rasterizerStateMgr{};

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

    auto vsBasic = DX11_GET_VERTEX_SHADER("VsBasic.cso");
    auto psGBuffer = DX11_GET_PIXEL_SHADER("PsGBuffer.cso");
    if (!vsBasic || !psGBuffer)
        return EXIT_FAILURE;

    FullscreenPass fullscreenPasses[] =
    {
        DX11_GET_PIXEL_SHADER("PsLightAmbient.cso"),
        DX11_GET_PIXEL_SHADER("PsGBufferWorldPosition.cso"),
        DX11_GET_PIXEL_SHADER("PsGBufferWorldNormal.cso"),
    };

    if (!std::ranges::all_of(fullscreenPasses, &FullscreenPass::operator bool))
        return EXIT_FAILURE;

    D3D11_SAMPLER_DESC samplerDesc{};
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    ComPtr<ID3D11SamplerState> sampler{};
    dx11.GetDevice()->CreateSamplerState(&samplerDesc, &sampler);
    if (!sampler)
        return EXIT_FAILURE;

    dx11.GetContext()->PSSetSamplers(0, 1, sampler.GetAddressOf());

    ScopedTopology topology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

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

        /*
        * Begin rendering
        */

        // Clearing
        swapChain.ClearRenderTarget();
        geometryBuffer.ClearRenderTargets();
        depthBuffer.ClearDepth();

        // Set camera
        {
            auto& cameras = scene->GetCameras();
            if (!cameras.empty())
            {
                auto& [camera, transform] = cameras.front();
                camera.SetCamera(transform->GetWorldMatrix());
            }
        }

        // Shaders
        vsBasic->SetShader();
        psGBuffer->SetShader();

        // Render scene
        geometryBuffer.SetRenderTargets(depthBuffer.GetView());
        {
            auto& meshes = scene->GetMeshes();
            for (auto& [mesh, transforms] : meshes)
            {
                for (auto& transform : transforms)
                    mesh.Draw(transform->GetWorldMatrix());
            }
        }
        dx11.GetContext()->OMSetRenderTargets(0, NULL, NULL);

        static int pass{};

        // Render from gbuffer to backbuffer
        swapChain.SetRenderTarget();
        {
            ScopedResourcesPs resources{ 0, geometryBuffer };
            fullscreenPasses[pass].Draw();
        }


        /*
        * End rendering
        */

        // ImGui
        {
            imGui.NewFrame();

            if (ImGui::Begin("Buffer"))
            {
                ImGui::SliderInt("Buffer", &pass, 0, static_cast<int>(std::size(fullscreenPasses)) - 1);
            }
            ImGui::End();

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