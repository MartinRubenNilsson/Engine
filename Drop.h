#pragma once

class Drop
{
public:
	Drop(HDROP);

	std::vector<fs::path> GetPaths() const;
	POINT GetPoint() const;

private:
	using Element = std::remove_pointer_t<HDROP>;
	using Deleter = decltype(DragFinish);

	std::unique_ptr<Element, Deleter*> myDrop;
};

