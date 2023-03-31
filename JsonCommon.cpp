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
	using value_t = json::value_t;

	value_t type = j.type();

	PushItemWidth(-1);
	if (Combo<value_t, std::to_underlying(value_t::number_float) + 1>("##Type", type))
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
		Text("Array"); // todo
		break;
	case value_t::string:
		InputText("##String", j.get_ptr<std::string*>());
		break;
	case value_t::boolean:
		Checkbox("##Boolean", j.get_ptr<bool*>());
		break;
	case value_t::number_integer:
		InputScalar("##Integer", ImGuiDataType_S64, j.get_ptr<int64_t*>());
		break;
	case value_t::number_unsigned:
		InputScalar("##Unsigned", ImGuiDataType_U64, j.get_ptr<uint64_t*>());
		break;
	case value_t::number_float:
		InputDouble("##Float", j.get_ptr<double*>());
		break;
	}
}
