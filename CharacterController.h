#pragma once

class CharacterController
{
public:
	CharacterController();

	explicit operator bool() const;

private:
	PxPtr<PxCapsuleController> myImpl{};
};

namespace ImGui
{
	void Inspect(CharacterController&);
}

