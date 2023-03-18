#include "pch.h"
#include "PlayControls.h"

void ImGui::PlayControls(PlayState& aState)
{
    switch (aState)
    {
    case PlayState::Stopped:
    {
        if (Button(ICON_FA_PLAY))
            aState = PlayState::Starting;
        break;
    }
    case PlayState::Started:
    {
        if (Button(ICON_FA_PAUSE))
            aState = PlayState::Stopping;
        break;
    }
    }
}
