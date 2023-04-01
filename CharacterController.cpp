#include "pch.h"
#include "CharacterController.h"
#include "PhysX.h"

CharacterController::CharacterController()
{
	PxCapsuleControllerDesc desc{};
	desc.material = PhysX::Get().GetDefaultMaterial();
	desc.radius = 0.5f;
	desc.height = 2.f;
	if (!desc.isValid())
		return;

	PxController* controller = PhysX::Get().GetControllerMgr()->createController(desc);
	myImpl.reset(static_cast<PxCapsuleController*>(controller));
}

CharacterController::operator bool() const
{
	return myImpl.operator bool();
}

/*
* namespace ImGui
*/

void ImGui::Inspect(CharacterController&)
{
	// todo
}
