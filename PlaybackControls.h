#pragma once

enum class PlayMode // todo: use for something
{
    Play = 1 << 0,
    Pause = 1 << 1,
    _entt_enum_as_bitmask
};

namespace ImGui
{
    void PlaybackControls(PlayMode&);
}

