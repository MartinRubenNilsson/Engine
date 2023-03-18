#include "pch.h"
#include "Manipulator.h"
#include "EnttCommon.h"
#include "Camera.h"
#include "Transform.h"

void ImGui::Manipulator(entt::registry& aRegistry, Camera& aCamera, Matrix& aCameraTransform)
{
    using namespace ImGuizmo;

    static OPERATION operation = TRANSLATE;
    static MODE mode = LOCAL;

    if (Mouse::Get().GetState().positionMode == Mouse::MODE_ABSOLUTE)
    {
        if (IsKeyPressed(ImGuiKey_W))
            operation = TRANSLATE;
        if (IsKeyPressed(ImGuiKey_E))
            operation = ROTATE;
        if (IsKeyPressed(ImGuiKey_R))
            operation = SCALE;

        if (IsKeyPressed(ImGuiKey_X))
            mode = static_cast<MODE>(1 - mode);
    }

    SetOrthographic(aCamera.GetType() == CameraType::Orthographic);

    if (auto transform = aRegistry.try_get<Transform>(GetSelectedFront(aRegistry)))
    {
        Matrix view = aCameraTransform.Invert() * GetDefaultViewMatrix();
        Matrix proj = aCamera.GetProjMatrix();
        Matrix world = transform->GetWorldMatrix(aRegistry);

        if (Manipulate(&view._11, &proj._11, operation, mode, &world._11))
            transform->SetWorldMatrix(aRegistry, world);
    }
}
