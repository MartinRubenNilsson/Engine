#pragma once

namespace ImGui
{
	bool BeginDrapDropTargetWindow(const char* payload_type);

	template <class Enum>
		requires std::is_scoped_enum_v<Enum>
	void Combo(const char* aLabel, Enum& aValue)
	{
		if (BeginCombo(aLabel, ToString(aValue)))
		{
			for (std::underlying_type_t<Enum> i = 0; i < std::to_underlying(Enum::Count); ++i)
			{
				Enum value{ i };
				if (Selectable(ToString(value), value == aValue))
					aValue = value;
			}
			EndCombo();
		}
	}
}