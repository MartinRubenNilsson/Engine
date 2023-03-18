#include "pch.h"
#include "PlaybackControls.h"

void ImGui::PlaybackControls(PlayMode& aMode)
{
    static const ImU32 defaultColor = GetColorU32(ImGuiCol_Button);
    static const ImU32 activeColor = GetColorU32(ImGuiCol_ButtonActive);

    bool playing = (aMode & PlayMode::Play) == PlayMode::Play;
    bool paused = (aMode & PlayMode::Pause) == PlayMode::Pause;

    PushStyleColor(ImGuiCol_Button, playing ? activeColor : defaultColor);
    if (Button(ICON_FA_PLAY))
        aMode ^= PlayMode::Play;
    PopStyleColor();

    SameLine();

    PushStyleColor(ImGuiCol_Button, paused ? activeColor : defaultColor);
    if (Button(ICON_FA_PAUSE))
        aMode ^= PlayMode::Pause;
    PopStyleColor();
}
