#pragma once

#include "texture.h"
#include "vector2d.h"

namespace king {

    struct Sprite {
        Texture &texture;
        Vector2D position{};
    };

} // namespace king
