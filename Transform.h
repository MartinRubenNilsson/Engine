#pragma once

class Transform
{
public:
	using Ptr = std::shared_ptr<Transform>;

	Transform(const aiNode&);

	const Matrix& GetMatrix() { return myMatrix; }
	void SetMatrix(const Matrix& aMatrix) { myMatrix = aMatrix; }

	std::vector<Matrix> GetHierarchyWorldMatrices() const;

	size_t GetChildCount() const { return myChildren.size(); }
	size_t GetDescendantCount() const;
	size_t GetHierarchyCount() const { return GetDescendantCount() + 1; }

	bool HasChildren() const { return !myChildren.empty(); }

private:
	std::span<Matrix> GetHierarchyWorldMatrices(std::span<Matrix>) const;

	std::string myName;
	Matrix myMatrix;
	std::vector<Ptr> myChildren;
};

