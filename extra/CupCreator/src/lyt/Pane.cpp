//
// Created by Gabi on 3/24/2024.
//
#include "Pane.h"

namespace gablibela::Layout {
    Pane::Pane(low::brlyt::pan1 &pan) {
        isVisible = TestBit(pan.flag, low::lyt::PANEFLAG_VISIBLE);
        influencedAlpha = TestBit(pan.flag, low::lyt::PANEFLAG_INFLUENCEDALPHA);
        adjustLocation = TestBit(pan.flag, low::lyt::PANEFLAG_LOCATIONADJUST);

        translation = math::Vector3(pan.translate);
        rotation = math::Vector3(pan.rotate);
        scale = math::Vector2(pan.scale);
        size = math::Vector2(pan.size.width, pan.size.width);

        horizontalPosition = GetHorizontalPosition(pan.basePos);
        verticalPosition = GetVerticalPosition(pan.basePos);

        alpha = ((float)pan.alpha) / UINT8_MAX;
    }

    void Pane::Render() {

    }
}
