#include "pch.h"
#include "Manipulator.h"
#include "EnttCommon.h"
#include "Camera.h"
#include "Transform.h"

void ImGui::Shortcut(OPERATION& anOp)
{
    if (IsKeyPressed(ImGuiKey_Q))
        anOp = TRANSLATE;
    if (IsKeyPressed(ImGuiKey_W))
        anOp = ROTATE;
    if (IsKeyPressed(ImGuiKey_E))
        anOp = SCALE;
    if (IsKeyPressed(ImGuiKey_R))
        anOp = TRANSLATE | ROTATE | SCALE;
}

void ImGui::Shortcut(MODE& aMode)
{
    if (IsKeyPressed(ImGuiKey_X))
        aMode = static_cast<MODE>(1 - aMode);
}

void ImGui::Manipulator(entt::registry& aRegistry, const Camera& aCam, const Matrix& aCamTrans, OPERATION& op, MODE& mode)
{
    if (RadioButton(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, op == TRANSLATE))
        op = TRANSLATE;
    if (RadioButton(ICON_FA_ROTATE, op == ROTATE))
        op = ROTATE;
    if (RadioButton(ICON_FA_SQUARE_ARROW_UP_RIGHT, op == SCALE))
        op = SCALE;
    if (RadioButton(ICON_FA_GROUP_ARROWS_ROTATE, op == (TRANSLATE | ROTATE | SCALE)))
        op = (TRANSLATE | ROTATE | SCALE);

    Separator();

    if (RadioButton(ICON_FA_CUBE, mode == LOCAL))
        mode = LOCAL;
    if (RadioButton(ICON_FA_GLOBE, mode == WORLD))
        mode = WORLD;

    SetOrthographic(aCam.GetType() == CameraType::Orthographic);

    if (auto transform = aRegistry.try_get<Transform>(GetSelected(aRegistry)))
    {
        Matrix view = aCamTrans.Invert() * GetDefaultViewMatrix();
        Matrix proj = aCam.GetProjMatrix();
        Matrix world = transform->GetWorldMatrix(aRegistry);

        if (Manipulate(&view._11, &proj._11, op, mode, &world._11))
            transform->SetWorldMatrix(aRegistry, world);
    }
}
