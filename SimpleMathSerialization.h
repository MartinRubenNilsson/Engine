#pragma once

namespace DirectX
{
    namespace SimpleMath
    {
        void to_json(json&, const Vector3&);
        void from_json(const json&, Vector3&);
        void to_json(json&, const Quaternion&);
        void from_json(const json&, Quaternion&);
	}
}

