#include <Ghost/GhostManager.hpp>

void CorrectGhostTrackName(LayoutUIControl * control, const char *textBoxName, u32 bmgId, const TextInfo *text)
{
    u32 trackId = LeCode::LeCodeManager::GetStaticInstance()->GetTrackID();
    bmgId = trackId + 0x7000;
    control->SetTextBoxMsg(textBoxName, bmgId, text);
    return;
}

kmCall(0x805e2a4c, CorrectGhostTrackName);
