#pragma once

class Drop
{
public:
	Drop();
	Drop(HDROP);

	std::vector<fs::path> GetPaths() const;
	POINT GetPoint() const;

	explicit operator bool() const;

private:
	using Element = std::remove_pointer_t<HDROP>;
	using Deleter = void(HDROP);

	std::unique_ptr<Element, Deleter*> myDrop;
};

