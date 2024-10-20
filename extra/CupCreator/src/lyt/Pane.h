//
// Created by Gabi on 3/24/2024.
//

#ifndef UIEDITOR_PANE_H
#define UIEDITOR_PANE_H

#include "../low/brlyt.h"
#include "../math/gMath.h"
#include <cstdint>
#include <vector>
#include <string>


namespace gablibela::Layout {

    enum HORIZONTAL_POSITION
    {
        HORIZONTALPOSITION_LEFT,
        HORIZONTALPOSITION_CENTER,
        HORIZONTALPOSITION_RIGHT,

        HORIZONTALPOSITION_MAX
    };

    enum VERTICAL_POSITION
    {
        VERTICALPOSITION_TOP,
        VERTICALPOSITION_CENTER,
        VERTICALPOSITION_BOTTOM,

        VERTICALPOSITION_MAX
    };

    template <typename T>
    inline bool TestBit(T bits, int pos) { const T mask = T(1 << pos); return (bits & mask) != 0; }

    class Pane {
    public:
        Pane() = default;
        Pane(low::brlyt::pan1& pan);

        virtual void Render();

        static inline HORIZONTAL_POSITION GetHorizontalPosition(uint8_t var) { return (HORIZONTAL_POSITION) (var % HORIZONTALPOSITION_MAX); }
        static inline VERTICAL_POSITION GetVerticalPosition(uint8_t var) { return (VERTICAL_POSITION) (var / VERTICALPOSITION_MAX); }

    private:
        std::string name;
        std::string userData;
        bool isVisible;
        bool influencedAlpha;
        bool adjustLocation;

        float alpha;

        HORIZONTAL_POSITION horizontalPosition;
        VERTICAL_POSITION verticalPosition;
        math::Vector3 translation{};
        math::Vector3 rotation{};
        math::Vector2 scale{};
        math::Vector2 size{};

        Pane * parent = nullptr;
        std::vector<Pane> children{};
    };
}




#endif //UIEDITOR_PANE_H
