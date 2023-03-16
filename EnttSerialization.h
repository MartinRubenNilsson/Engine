#pragma once

namespace entt
{
	void to_json(json&, const registry&);
	void from_json(const json&, registry&);
}

