#include "pch.h"
#include "JsonCommon.h"
#include "ImGuiCommon.h"

const char* ToString(json::value_t aType)
{
    static constexpr std::array strings
    {
        "Null",
        "Object",
        "Array",
        "String",
        "Boolean",
        "Integer",
        "Unsigned",
        "Float",
        "Binary",
        "Discarded",
    };

    return strings.at(std::to_underlying(aType));
}

void ImGui::Inspect(json& j)
{
	PushID(&j);

	using value_t = json::value_t;

	value_t type = j.type();

	PushItemWidth(85.f);
	if (Combo<value_t, 8>("##Type", type))
		j = (type);
	PopItemWidth();

	switch (j.type())
	{
	case value_t::null:
		break;
	case value_t::object:
		for (auto& [key, value] : j.items())
		{
			if (TreeNode(key.c_str()))
			{
				Inspect(value);
				TreePop();
			}
		}
		break;
	case value_t::array:
		for (size_t i = 0; i < j.size(); ++i)
		{
			PushID(i);
			bool erase = SmallButton("-");
			PopID();
			SameLine();
			if (TreeNode(std::to_string(i).c_str()))
			{
				Inspect(j.at(i));
				TreePop();
			}
			if (erase)
			{
				j.erase(i);
				break;
			}
		}
		if (SmallButton("+"))
			j.emplace_back();
		break;
	case value_t::string:
		SameLine();
		InputText("##String", j.get_ptr<std::string*>());
		break;
	case value_t::boolean:
		SameLine();
		Checkbox("##Boolean", j.get_ptr<bool*>());
		break;
	case value_t::number_integer:
		SameLine();
		InputScalar("##Integer", ImGuiDataType_S64, j.get_ptr<int64_t*>());
		break;
	case value_t::number_unsigned:
		SameLine();
		InputScalar("##Unsigned", ImGuiDataType_U64, j.get_ptr<uint64_t*>());
		break;
	case value_t::number_float:
		SameLine();
		InputDouble("##Float", j.get_ptr<double*>());
		break;
	}

	PopID();
}
