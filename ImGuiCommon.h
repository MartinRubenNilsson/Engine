#pragma once

namespace ImGui
{
	bool BeginDrapDropTargetWindow(const char* payload_type);

	template <class Enum, auto Count = std::to_underlying(Enum::Count)>
		requires std::is_enum_v<Enum>
	bool Combo(const char* aLabel, Enum& aValue)
	{
		bool valueChanged = false;
		if (BeginCombo(aLabel, ToString(aValue)))
		{
			for (std::underlying_type_t<Enum> i = 0; i < Count; ++i)
			{
				Enum value{ i };
				if (Selectable(ToString(value), value == aValue))
				{
					if (value != aValue)
						valueChanged = true;
					aValue = value;
				}
			}
			EndCombo();
		}
		return valueChanged;
	}
}