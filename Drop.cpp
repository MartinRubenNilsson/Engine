#include "pch.h"
#include "Drop.h"

namespace
{
    HDROP theDrop = nullptr;
}

/*
* namespace Drop
*/

void Drop::Accept(HDROP hDrop)
{
    theDrop = hDrop;
}

bool Drop::Begin()
{
    return theDrop;
}

void Drop::End()
{
    DragFinish(theDrop);
    theDrop = nullptr;
}

std::vector<fs::path> Drop::GetPaths()
{
    std::vector<fs::path> paths{};

    for (UINT i = 0, count = DragQueryFile(theDrop, 0xFFFFFFFF, NULL, 0); i < count; ++i)
    {
        WCHAR path[MAX_PATH]{};
        DragQueryFile(theDrop, i, path, MAX_PATH);
        paths.emplace_back(path);
    }

    return paths;
}

POINT Drop::GetPoint()
{
    POINT point{};
    DragQueryPoint(theDrop, &point);
    return point;
}
