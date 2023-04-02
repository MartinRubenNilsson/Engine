#include "pch.h"
#include "SimpleMathSerialization.h"

void DirectX::SimpleMath::to_json(json& j, const Vector3& v)
{
	j = { v.x, v.y, v.z };
}

void DirectX::SimpleMath::from_json(const json& j, Vector3& v)
{
	j.at(0).get_to(v.x);
	j.at(1).get_to(v.y);
	j.at(2).get_to(v.z);
}

void DirectX::SimpleMath::to_json(json& j, const Quaternion& q)
{
	j = { q.x, q.y, q.z, q.w };
}

void DirectX::SimpleMath::from_json(const json& j, Quaternion& q)
{
	j.at(0).get_to(q.x);
	j.at(1).get_to(q.y);
	j.at(2).get_to(q.z);
	j.at(3).get_to(q.w);
}
