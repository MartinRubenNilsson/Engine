#include "pch.h"
#include "Window.h"
#include "Drop.h"
#include "DearImGui.h"
#include "BackBuffer.h"
#include "DepthBuffer.h"
#include "GeometryBuffer.h"
#include "StateManager.h"
#include "InputLayoutManager.h"
#include "Scene.h"
#include "FullscreenPass.h"
#include "Scopes.h"
#include "Image.h"
#include "Cubemap.h"
#include "Camera.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
    std::unique_ptr<Drop> theDrop;
    std::shared_ptr<const Scene> theScene;
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

    WindowClass windowClass{ WndProc };
    Window window{ windowClass };
    if (!window)
        return EXIT_FAILURE;

    window.SetTitle(L"Model Viewer");

    DX11 dx11{};
    if (!dx11)
        return EXIT_FAILURE;

    DearImGui imGui{ window, dx11.GetDevice(), dx11.GetContext() };
    if (!imGui)
        return EXIT_FAILURE;

    InputLayoutManager inputLayoutMgr{};
    if (!inputLayoutMgr)
        return EXIT_FAILURE;

    ConstantBufferManager constantBufferMgr{};
    if (!constantBufferMgr)
        return EXIT_FAILURE;

    StateManager stateMgr{};
    ShaderManager shaderMgr{};

    BackBuffer backBuffer{ window };
    if (!backBuffer)
        return EXIT_FAILURE;

    GeometryBuffer geometryBuffer{ backBuffer.GetWidth(), backBuffer.GetHeight() };
    if (!geometryBuffer)
        return EXIT_FAILURE;

    DepthBuffer depthBuffer{ backBuffer.GetWidth(), backBuffer.GetHeight() };
    if (!depthBuffer)
        return EXIT_FAILURE;

    FullscreenPass fullscreenPasses[] =
    {
        PIXEL_SHADER("PsLightAmbient.cso"),
        PIXEL_SHADER("PsGBufferWorldPosition.cso"),
        PIXEL_SHADER("PsGBufferWorldNormal.cso"),
        PIXEL_SHADER("PsGBufferDiffuse.cso"),
        PIXEL_SHADER("PsGBufferMetalRoughAo.cso"),
    };

    if (!std::ranges::all_of(fullscreenPasses, &FullscreenPass::operator bool))
        return EXIT_FAILURE;

    D3D11_SAMPLER_DESC samplerDesc{ CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} } };

    Viewport viewport{};
    viewport.width = static_cast<float>(backBuffer.GetWidth());
    viewport.height = static_cast<float>(backBuffer.GetHeight());
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

    SceneManager sceneMgr{};

    PerspectiveCamera perspective{};
    perspective.fovY = 1.04719755119f; // 60 degrees
    perspective.aspect = viewport.AspectRatio();
    Camera camera{ perspective };

    Matrix cameraTransform;
    const float cameraDistance = 20.f;
    cameraTransform.Translation({ 0.f, 0.f, -cameraDistance });

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

        // Drag and drop
        if (theDrop)
        {
            theScene = sceneMgr.GetScene(theDrop->GetPaths().front());
            theDrop = nullptr;
        }

        // Rendering
        backBuffer.Clear();
        geometryBuffer.Clear();
        depthBuffer.Clear();

        ScopedRenderTargets swapChainScope{ backBuffer };

        camera.SetCamera(cameraTransform);

        if (theScene)
        {
            ScopedInputLayout layout{ typeid(BasicVertex) };
            ScopedShader vs{ VERTEX_SHADER("VsBasic.cso") };
            ScopedShader ps{ PIXEL_SHADER("PsGBuffer.cso") };
            ScopedRenderTargets geometryBufferScope{ geometryBuffer, depthBuffer };

            auto& materials = theScene->GetMaterials();
            auto& meshes = theScene->GetMeshes();

            for (auto& [mesh, transforms] : meshes)
            {
                ScopedShaderResources resources{ ShaderType::Pixel, 10, materials[mesh.GetMaterialIndex()] };

                for (auto& transform : transforms)
                    mesh.Draw(transform->GetWorldMatrix());
            }
        }

        static int pass{};

        {
            ScopedShaderResources resources{ ShaderType::Pixel, 0, geometryBuffer };
            fullscreenPasses[pass].Draw();
        }
        {
            ScopedRenderTargets skyboxScope{ backBuffer, depthBuffer };
            cubemap.DrawSkybox();
        }

        // ImGui
        {
            imGui.NewFrame();

            if (ImGui::Begin("Buffer"))
                ImGui::SliderInt("Buffer", &pass, 0, static_cast<int>(std::size(fullscreenPasses)) - 1);
            ImGui::End();

            ImGui::ViewManipulate(camera, cameraTransform, cameraDistance, {}, { 150.f, 150.f }, 0);

            if (theScene)
            {
                ImGui::Begin("Materials");
                for (const auto& material : theScene->GetMaterials())
                {
                    if (ImGui::TreeNode(material.GetName().data()))
                    {
                        ImGui::InspectMaterial(material);
                        ImGui::TreePop();
                    }
                }
                ImGui::End();

                static Transform::Ptr selection;

                ImGui::Begin("Hierarchy");
                ImGui::Hierarchy(theScene->GetRootTransform(), selection);
                ImGui::End();

                if (selection)
                {
                    auto operation = ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE;
                    auto mode = ImGuizmo::LOCAL;

                    Matrix m = selection->GetWorldMatrix();
                    ImGui::Manipulate(camera, cameraTransform, operation, mode, m);
                    selection->SetWorldMatrix(m);
                }
            }

            imGui.Render();
        }

        backBuffer.Present();
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
    case WM_DROPFILES:
        theDrop = std::make_unique<Drop>((HDROP)wParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(EXIT_SUCCESS);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}
