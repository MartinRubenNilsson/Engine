#pragma once

enum class PlayState
{
    Stopped,
    Starting,
    Started,
    Stopping
};

namespace ImGui
{
    void PlayControls(PlayState&);
}

