#include "pch.h"
#include "Window.h"
#include "DearImGui.h"
#include "SwapChain.h"
#include "Shader.h"
#include "InputLayout.h"
#include "Mesh.h"
#include "DepthBuffer.h"
#include "Camera.h"
#include "Transform.h"
#include "imgui_simplemath.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    WindowClass windowClass{ WndProc };
	Window window{ windowClass };
	if (!window)
		return EXIT_FAILURE;

    window.SetTitle(L"Model Viewer");

    DX11 dx11{};
    if (!dx11)
        return EXIT_FAILURE;

    DearImGui imGui{ window.GetHandle(), dx11.GetDevice(), dx11.GetContext() };
    if (!imGui)
        return EXIT_FAILURE;

    SwapChain swapChain{ window.GetHandle() };
    if (!swapChain)
        return EXIT_FAILURE;

    DepthBuffer depthBuffer{ swapChain.GetWidth(), swapChain.GetHeight() };
    if (!depthBuffer)
        return EXIT_FAILURE;

    ConstantBufferManager cBufferMgr{};
    if (!cBufferMgr)
        return EXIT_FAILURE;

    const fs::path currentPath = fs::current_path() / "Bin";
    fs::current_path(currentPath);




    Transform::Ptr rootTransform{};

    std::vector<std::pair<Camera::Ptr, Transform::Ptr>> cameraInstances{};
    std::vector<std::pair<Mesh::Ptr, Transform::Ptr>> meshInstances{};

    {
        Assimp::Importer importer{};
        const unsigned flags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality;

        if (auto scene = importer.ReadFile("mesh/test_00.fbx", flags))
        {
            rootTransform = Transform::Create(*scene->mRootNode);

            for (auto aiCamera : std::span(scene->mCameras, scene->mNumCameras))
            {
                auto camera = Camera::Create(*aiCamera);
                auto transform = rootTransform->FindInHierarchy(aiCamera->mName.C_Str());

                cameraInstances.emplace_back(camera, transform);
            }

            std::vector<Mesh::Ptr> meshes{};

            for (auto aiMesh : std::span(scene->mMeshes, scene->mNumMeshes))
                meshes.emplace_back(Mesh::Create(*aiMesh));

            //for (auto aiNode : todo) // TODO
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

    /*Camera camera{};
    Matrix cameraTransform{};*/

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

        if (ImGui::Begin("Hierarchy"))
        {
            ImGui::Hierarchy("Hierarchy", rootTransform);
            ImGui::End();
        }

        /*if (!cameras.empty())
            cameras.front().UseForDrawing(cameraTransforms.front()->GetWorldMatrix());

        auto worldMatrices = rootTransform->GetHierarchyWorldMatrices();

        for (size_t i = 0; i < meshes.size(); ++i)
            meshes[i].Draw(worldMatrices[i + 1]);*/

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