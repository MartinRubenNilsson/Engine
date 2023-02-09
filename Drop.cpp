#include "pch.h"
#include "Drop.h"

Drop::Drop(HDROP hDrop)
	: myDrop{ hDrop, DragFinish }
{
}

std::vector<fs::path> Drop::GetPaths() const
{
    std::vector<fs::path> paths{ DragQueryFile(myDrop.get(), 0xFFFFFFFF, NULL, 0) };

    for (UINT i = 0; i < paths.size(); ++i)
    {
        WCHAR path[MAX_PATH]{};
        DragQueryFile(myDrop.get(), i, path, MAX_PATH);
        paths[i] = path;
    }

    return paths;
}

POINT Drop::GetPoint() const
{
    POINT point{};
    DragQueryPoint(myDrop.get(), &point);
    return point;
}
