#pragma once

enum class PlayState
{
    Stopped,
    Starting,
    Started,
    Stopping,
    Quit,
};

namespace ImGui
{
    void PlayControls(PlayState&);
}

